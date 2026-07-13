/* CustomLogic.cpp - Beginner-friendly CCL logic gates for WazamonoCore. */
#include "CustomLogic.h"

/* ---- board unit -> LUT/pin assignment --------------------------------- */
#if defined(WAZAMONO_BOARD_KUNAI)
static const uint8_t unit0_pins[4] = {PIN_PA0, PIN_PA1, PIN_PA2, PIN_PA3}; /* D4, D5, D3 -> D2 */
CustomLogicClass CustomLogic(0, unit0_pins);   /* LUT0 */
#else /* Tachi / Tsurugi */
static const uint8_t unit0_pins[4] = {PIN_PD0, PIN_PD1, PIN_PD2, PIN_PD3};
static const uint8_t unit1_pins[4] = {PIN_PF0, PIN_PF1, PIN_PF2, PIN_PF3};
CustomLogicClass CustomLogic(2, unit0_pins);   /* LUT2 */
CustomLogicClass CustomLogic1(3, unit1_pins);  /* LUT3 */
#endif

/* Which LUTs are running (for global CCL enable management) and the
 * per-LUT interrupt callbacks, shared by the single CCL vector. */
static uint8_t s_activeMask = 0;
static void (*s_callbacks[4])() = {nullptr, nullptr, nullptr, nullptr};

/* LUTn register block: CTRLA, CTRLB, CTRLC, TRUTH - 4 bytes per LUT. */
static volatile uint8_t *lutBase(uint8_t lut) {
  return (&CCL.LUT0CTRLA) + (uint8_t)(lut << 2);
}

/* One binary logic operation. NOT must be filtered out by the caller. */
static bool applyOp(LogicType op, bool a, bool b) {
  switch (op) {
    case AND:  return  (a && b);
    case OR:   return  (a || b);
    case XOR:  return  (a != b);
    case NAND: return !(a && b);
    case NOR:  return !(a || b);
    case XNOR: return  (a == b);
    default:   return false;
  }
}

/* ---- class ------------------------------------------------------------ */
CustomLogicClass::CustomLogicClass(uint8_t lut, const uint8_t *pins)
  : _pins(pins), _lut(lut) {}

bool CustomLogicClass::configure(uint8_t truth, uint8_t inputMask) {
  if (inputMask == 0 || inputMask > 0x07) return false;
  volatile uint8_t *base = lutBase(_lut);

  base[0] = 0;  /* disable this LUT: CTRLB/CTRLC/TRUTH are enable-protected */

  /* Input pins: used inputs get pull-ups so buttons to GND work out of
   * the box; a driven logic signal simply overrides the pull-up. */
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(_pins[i], (inputMask & (1 << i)) ? INPUT_PULLUP : INPUT);
  }

  base[1] = ((inputMask & 0x01) ? CCL_INSEL0_IN0_gc : CCL_INSEL0_MASK_gc)   /* CTRLB */
          | ((inputMask & 0x02) ? CCL_INSEL1_IN1_gc : CCL_INSEL1_MASK_gc);
  base[2] =  (inputMask & 0x04) ? CCL_INSEL2_IN2_gc : CCL_INSEL2_MASK_gc;   /* CTRLC */
  base[3] = truth;                       /* TRUTH                 */
  base[0] = CCL_OUTEN_bm | CCL_ENABLE_bm; /* result drives the OUT pin */

  s_activeMask |= (1 << _lut);
  CCL.CTRLA |= CCL_ENABLE_bm;
  _numInputs = __builtin_popcount(inputMask);
  _running = true;
  return true;
}

bool CustomLogicClass::begin(LogicType logic1) {
  if (logic1 > NOP) return false;
  if (logic1 == NOT || logic1 == NOP) {
    /* one input: NOT = inverter on IN0, NOP = buffer on IN0 */
    uint8_t truth = 0;
    for (uint8_t i = 0; i < 8; i++) {
      bool in0 = (i & 1);
      if ((logic1 == NOP) ? in0 : !in0) truth |= (1 << i);
    }
    return configure(truth, 0b001);
  }
  /* OUT = IN0 (logic1) IN1 */
  uint8_t truth = 0;
  for (uint8_t i = 0; i < 8; i++) {
    if (applyOp(logic1, i & 1, i & 2)) truth |= (1 << i);
  }
  return configure(truth, 0b011);
}

bool CustomLogicClass::begin(LogicType logic1, LogicType logic2) {
  if (logic1 == NOT || logic2 == NOT) return false; /* NOT cannot combine   */
  if (logic1 > NOP  || logic2 > NOP)  return false;
  if (logic1 == NOP && logic2 == NOP) return false; /* meaningless          */
  if (logic2 == NOP) return begin(logic1);          /* skip IN2 = 2-input   */
  uint8_t truth = 0;
  if (logic1 == NOP) {
    /* skip IN0: OUT = IN1 (logic2) IN2 */
    for (uint8_t i = 0; i < 8; i++) {
      if (applyOp(logic2, i & 2, i & 4)) truth |= (1 << i);
    }
    return configure(truth, 0b110);
  }
  /* OUT = (IN0 logic1 IN1) logic2 IN2 */
  for (uint8_t i = 0; i < 8; i++) {
    if (applyOp(logic2, applyOp(logic1, i & 1, i & 2), i & 4)) truth |= (1 << i);
  }
  return configure(truth, 0b111);
}

bool CustomLogicClass::beginTruthTable(uint8_t truthTable, uint8_t numInputs) {
  if (numInputs < 1 || numInputs > 3) return false;
  return configure(truthTable, (1 << numInputs) - 1); /* IN0..IN(n-1) */
}

void CustomLogicClass::end() {
  detachInterrupt();
  lutBase(_lut)[0] = 0;
  for (uint8_t i = 0; i < 3; i++) pinMode(_pins[i], INPUT);
  s_activeMask &= ~(1 << _lut);
  if (s_activeMask == 0) CCL.CTRLA &= ~CCL_ENABLE_bm;
  _running = false;
}

bool CustomLogicClass::read() {
  return digitalRead(_pins[3]) == HIGH;
}

void CustomLogicClass::attachInterrupt(void (*callback)(), uint8_t mode) {
  uint8_t intmode;
  switch (mode) {
    case RISING:  intmode = CCL_INTMODE0_RISING_gc;  break;
    case FALLING: intmode = CCL_INTMODE0_FALLING_gc; break;
    default:      intmode = CCL_INTMODE0_BOTH_gc;    break; /* CHANGE */
  }
  s_callbacks[_lut] = callback;
  uint8_t shift = _lut << 1;                       /* 2 bits per LUT */
  CCL.INTFLAGS = (1 << _lut);                      /* clear a stale flag */
  CCL.INTCTRL0 = (CCL.INTCTRL0 & ~(0x03 << shift)) | (intmode << shift);
}

void CustomLogicClass::detachInterrupt() {
  uint8_t shift = _lut << 1;
  CCL.INTCTRL0 &= ~(0x03 << shift);
  s_callbacks[_lut] = nullptr;
}

ISR(CCL_CCL_vect) {
  uint8_t flags = CCL.INTFLAGS;
  CCL.INTFLAGS = flags;   /* clear */
  for (uint8_t lut = 0; lut < 4; lut++) {
    if ((flags & (1 << lut)) && s_callbacks[lut]) s_callbacks[lut]();
  }
}
