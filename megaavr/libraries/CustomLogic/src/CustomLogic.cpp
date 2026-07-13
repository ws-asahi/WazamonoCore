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

/* Is this LUT the even one of its pair? The CCL feeds back the output of the
 * even LUT (the sequencer is disabled here), so only an even LUT can see its
 * own output on the FEEDBACK input. */
static inline bool isEvenLut(uint8_t lut) {
  return (lut & 1) == 0;
}

/* The INSEL nibble for one input - identical encoding for INSEL0/1/2. */
uint8_t CustomLogicClass::insel(uint8_t input) {
  if (!(_inputMask & (1 << input))) {
    return CCL_INSEL0_MASK_gc;            /* input not used by the logic */
  }
  switch (_source[input]) {
    case LOGIC_ANALOG_COMP: return CCL_INSEL0_AC0_gc;
    case LOGIC_EVENT_A:     return CCL_INSEL0_EVENTA_gc;
    case LOGIC_EVENT_B:     return CCL_INSEL0_EVENTB_gc;
    case LOGIC_OWN_OUTPUT:  return CCL_INSEL0_FEEDBACK_gc;  /* even LUT only */
    case LOGIC_OTHER_UNIT:  /* LUT2 reaches LUT3 through LINK (LUT[n+1]);
                             * LUT3 reaches LUT2 through FEEDBACK (even LUT). */
                            return isEvenLut(_lut) ? CCL_INSEL0_LINK_gc
                                                   : CCL_INSEL0_FEEDBACK_gc;
    default:                return CCL_INSEL0_IN0_gc;       /* LOGIC_PIN */
  }
}

void CustomLogicClass::apply() {
  volatile uint8_t *base = lutBase(_lut);

  base[0] = 0;  /* disable this LUT: CTRLB/CTRLC/TRUTH are enable-protected */

  /* Pins: only an input that is actually fed from its pin gets claimed (with
   * a pull-up, so buttons to GND work out of the box; a driven logic signal
   * simply overrides it). Pins we do not use are left completely untouched -
   * they stay available for other peripherals. */
  for (uint8_t i = 0; i < 3; i++) {
    bool wantPin = (_inputMask & (1 << i)) && (_source[i] == LOGIC_PIN);
    if (wantPin) {
      pinMode(_pins[i], INPUT_PULLUP);
      _claimed |= (1 << i);
    } else if (_claimed & (1 << i)) {
      pinMode(_pins[i], INPUT);           /* release the pull-up we added */
      _claimed &= ~(1 << i);
    }
  }

  base[1] = insel(0) | (uint8_t)(insel(1) << 4);  /* CTRLB: INSEL0, INSEL1 */
  base[2] = insel(2);                             /* CTRLC: INSEL2         */
  base[3] = _truth;                               /* TRUTH                 */
  base[0] = CCL_OUTEN_bm | CCL_ENABLE_bm;         /* drive the OUT pin     */

  s_activeMask |= (1 << _lut);
  CCL.CTRLA |= CCL_ENABLE_bm;
}

bool CustomLogicClass::configure(uint8_t truth, uint8_t inputMask) {
  if (inputMask == 0 || inputMask > 0x07) return false;
  _truth = truth;
  _inputMask = inputMask;
  _numInputs = __builtin_popcount(inputMask);
  apply();
  _running = true;
  return true;
}

bool CustomLogicClass::setInput(uint8_t input, LogicInput source) {
  if (input > 2) return false;
  switch (source) {
    case LOGIC_PIN:
    case LOGIC_ANALOG_COMP:
    case LOGIC_EVENT_A:
    case LOGIC_EVENT_B:
      break;
    case LOGIC_OWN_OUTPUT:
      /* Only the even LUT of a pair sees its own output on FEEDBACK. */
      if (!isEvenLut(_lut)) return false;
      break;
    case LOGIC_OTHER_UNIT:
      #if defined(WAZAMONO_BOARD_KUNAI)
        return false;                    /* this board has a single unit */
      #else
        break;
      #endif
    default:
      return false;
  }
  _source[input] = source;
  if (_running) apply();                 /* take effect immediately */
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
  for (uint8_t i = 0; i < 3; i++) {
    if (_claimed & (1 << i)) pinMode(_pins[i], INPUT);   /* only ours */
  }
  _claimed = 0;
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
