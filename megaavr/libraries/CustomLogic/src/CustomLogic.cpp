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

/* Build the truth table for a standard gate over the low numInputs bits. */
static uint8_t makeTruth(uint8_t gate, uint8_t numInputs) {
  uint8_t mask = (1 << numInputs) - 1;
  uint8_t truth = 0;
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t bits = i & mask;
    uint8_t ones = __builtin_popcount(bits);
    bool v = false;
    switch (gate) {
      case AND:  v = (bits == mask);  break;
      case OR:   v = (bits != 0);     break;
      case XOR:  v = (ones & 1);      break;
      case NAND: v = (bits != mask);  break;
      case NOR:  v = (bits == 0);     break;
      case XNOR: v = !(ones & 1);     break;
      case NOT:  v = !(bits & 1);     break;
      default:   return 0;
    }
    if (v) truth |= (1 << i);
  }
  return truth;
}

/* ---- class ------------------------------------------------------------ */
CustomLogicClass::CustomLogicClass(uint8_t lut, const uint8_t *pins)
  : _pins(pins), _lut(lut) {}

bool CustomLogicClass::configure(uint8_t truth, uint8_t numInputs) {
  if (numInputs < 1 || numInputs > 3) return false;
  volatile uint8_t *base = lutBase(_lut);

  base[0] = 0;  /* disable this LUT: CTRLB/CTRLC/TRUTH are enable-protected */

  /* Input pins: inputs get pull-ups so buttons to GND work out of the box;
   * a driven logic signal simply overrides the pull-up. */
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(_pins[i], (i < numInputs) ? INPUT_PULLUP : INPUT);
  }

  base[1] = ((numInputs >= 1) ? CCL_INSEL0_IN0_gc : CCL_INSEL0_MASK_gc)   /* CTRLB */
          | ((numInputs >= 2) ? CCL_INSEL1_IN1_gc : CCL_INSEL1_MASK_gc);
  base[2] =  (numInputs >= 3) ? CCL_INSEL2_IN2_gc : CCL_INSEL2_MASK_gc;   /* CTRLC */
  base[3] = truth;                       /* TRUTH                 */
  base[0] = CCL_OUTEN_bm | CCL_ENABLE_bm; /* result drives the OUT pin */

  s_activeMask |= (1 << _lut);
  CCL.CTRLA |= CCL_ENABLE_bm;
  _numInputs = numInputs;
  _running = true;
  return true;
}

bool CustomLogicClass::begin(uint8_t gate, uint8_t numInputs) {
  if (gate > NOT) return false;
  if (gate == NOT) numInputs = 1;
  return configure(makeTruth(gate, numInputs), numInputs);
}

bool CustomLogicClass::beginTruthTable(uint8_t truthTable, uint8_t numInputs) {
  return configure(truthTable, numInputs);
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
