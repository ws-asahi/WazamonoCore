/* AnalogComp.cpp - Beginner-friendly analog comparator (AC0) for WazamonoCore. */
#include "AnalogComp.h"

AnalogCompClass AnalogComp;

/* ---- board default inputs ------------------------------------------- */
#if defined(WAZAMONO_BOARD_KUNAI)
  #define AC_DEFAULT_PLUS_PIN   PIN_PD6   /* D6, AINP3 */
  #define AC_DEFAULT_MINUS_PIN  PIN_PD7   /* D7, AINN2 */
#else /* Tachi / Tsurugi */
  #define AC_DEFAULT_PLUS_PIN   PIN_PD2   /* Tachi A1 / Tsurugi D9,  AINP0 */
  #define AC_DEFAULT_MINUS_PIN  PIN_PD3   /* Tachi A0 / Tsurugi D10, AINN0 */
#endif

/* ---- pin -> mux/PINCTRL mapping -------------------------------------- */
static bool plusPinToMux(uint8_t pin, uint8_t *gc) {
  #if defined(PIN_PD2)
  if (pin == PIN_PD2) { *gc = AC_MUXPOS_AINP0_gc; return true; }
  #endif
  if (pin == PIN_PD6) { *gc = AC_MUXPOS_AINP3_gc; return true; }
  /* AINP4 = PC3 is intentionally not offered: it is the VBUS divider on
   * Tachi/Kunai and the LED mirror on Tsurugi. */
  return false;
}

static bool minusPinToMux(uint8_t pin, uint8_t *gc) {
  #if defined(PIN_PD3)
  if (pin == PIN_PD3) { *gc = AC_MUXNEG_AINN0_gc; return true; }
  #endif
  #if defined(PIN_PD0)
  if (pin == PIN_PD0) { *gc = AC_MUXNEG_AINN1_gc; return true; }
  #endif
  if (pin == PIN_PD7) { *gc = AC_MUXNEG_AINN2_gc; return true; }
  return false;
}

/* Disable the digital input buffer of an analog input pin (all valid
 * comparator pins are on PORTD). */
static void setPinAnalog(uint8_t pin, bool analog) {
  uint8_t bit = digitalPinToBitPosition(pin);
  if (bit == NOT_A_PIN) return;
  volatile uint8_t *pinctrl = &((&PORTD.PIN0CTRL)[bit]);
  *pinctrl = analog ? PORT_ISC_INPUT_DISABLE_gc : 0;
}

/* ---- private helpers -------------------------------------------------- */
void AnalogCompClass::applyPins() {
  uint8_t mux = _muxpos_gc | (_useDacref ? AC_MUXNEG_DACREF_gc : _muxneg_gc);
  AC0.MUXCTRL = (AC0.MUXCTRL & AC_INVERT_bm) | mux;
  setPinAnalog(_plusPin, true);
  if (!_useDacref) setPinAnalog(_minusPin, true);
}

bool AnalogCompClass::applyThreshold(double volts) {
  /* Pick the smallest internal reference that can express the requested
   * voltage, for the finest resolution: Vdacref = (DACREF / 256) * Vref */
  struct { double v; uint8_t gc; } const refs[] = {
    {1.024, VREF_REFSEL_1V024_gc},
    {2.048, VREF_REFSEL_2V048_gc},
    {2.500, VREF_REFSEL_2V500_gc},
    {4.096, VREF_REFSEL_4V096_gc},
  };
  if (volts < 0) return false;
  for (uint8_t i = 0; i < 4; i++) {
    double maxv = refs[i].v * 255.0 / 256.0;
    if (volts <= maxv + 1e-9) {
      VREF.ACREF = VREF_ALWAYSON_bm | refs[i].gc;
      AC0.DACREF = (uint8_t)(volts * 256.0 / refs[i].v + 0.5);
      return true;
    }
  }
  return false; /* above 4.08 V */
}

/* ---- public API -------------------------------------------------------- */
bool AnalogCompClass::begin() {
  if (_plusPin == 255)  _plusPin  = AC_DEFAULT_PLUS_PIN;
  if (_minusPin == 255 && !_useDacref) _minusPin = AC_DEFAULT_MINUS_PIN;
  plusPinToMux(_plusPin, &_muxpos_gc);
  if (!_useDacref) minusPinToMux(_minusPin, &_muxneg_gc);
  applyPins();
  AC0.CTRLA |= AC_ENABLE_bm;
  _running = true;
  delayMicroseconds(20); /* let the comparator settle before the first read() */
  return true;
}

bool AnalogCompClass::begin(double thresholdVolts) {
  _useDacref = true;
  if (!applyThreshold(thresholdVolts)) { _useDacref = false; return false; }
  if (_minusPin != 255) { setPinAnalog(_minusPin, false); _minusPin = 255; }
  return begin();
}

void AnalogCompClass::end() {
  detachInterrupt();
  AC0.CTRLA = 0;
  AC0.MUXCTRL = 0;
  VREF.ACREF = 0;
  setPinAnalog(_plusPin, false);
  if (!_useDacref) setPinAnalog(_minusPin, false);
  _plusPin = 255; _minusPin = 255;
  _useDacref = false;
  _running = false;
}

bool AnalogCompClass::read() {
  return (AC0.STATUS & AC_CMPSTATE_bm) != 0;
}

bool AnalogCompClass::setInputs(uint8_t plusPin, uint8_t minusPin) {
  uint8_t p, n;
  if (!plusPinToMux(plusPin, &p) || !minusPinToMux(minusPin, &n)) return false;
  /* release previously configured pins */
  if (_plusPin != 255) setPinAnalog(_plusPin, false);
  if (_minusPin != 255) setPinAnalog(_minusPin, false);
  _plusPin = plusPin; _minusPin = minusPin;
  _muxpos_gc = p; _muxneg_gc = n;
  _useDacref = false;
  if (_running) applyPins();
  return true;
}

bool AnalogCompClass::setThreshold(double thresholdVolts) {
  if (!applyThreshold(thresholdVolts)) return false;
  if (_minusPin != 255) { setPinAnalog(_minusPin, false); _minusPin = 255; }
  _useDacref = true;
  if (_running) applyPins();
  return true;
}

void AnalogCompClass::setHysteresis(uint8_t level) {
  uint8_t ctrla = AC0.CTRLA & ~AC_HYSMODE_gm;
  switch (level) {
    case AC_HYST_SMALL:  ctrla |= AC_HYSMODE_SMALL_gc;  break;
    case AC_HYST_MEDIUM: ctrla |= AC_HYSMODE_MEDIUM_gc; break;
    case AC_HYST_LARGE:  ctrla |= AC_HYSMODE_LARGE_gc;  break;
    default: break; /* AC_HYST_NONE */
  }
  AC0.CTRLA = ctrla;
}

void AnalogCompClass::enableOutput(bool invert) {
  if (invert) AC0.MUXCTRL |=  AC_INVERT_bm;
  else        AC0.MUXCTRL &= ~AC_INVERT_bm;
  AC0.CTRLA |= AC_OUTEN_bm;   /* result appears on PA7 */
}

void AnalogCompClass::disableOutput() {
  AC0.CTRLA &= ~AC_OUTEN_bm;
}

void AnalogCompClass::attachInterrupt(void (*callback)(), uint8_t mode) {
  uint8_t intmode;
  switch (mode) {
    case RISING:  intmode = AC_INTMODE_POSEDGE_gc; break;
    case FALLING: intmode = AC_INTMODE_NEGEDGE_gc; break;
    default:      intmode = AC_INTMODE_BOTHEDGE_gc; break; /* CHANGE */
  }
  _callback = callback;
  AC0.STATUS = AC_CMP_bm;               /* clear a stale flag */
  AC0.INTCTRL = intmode | AC_CMP_bm;    /* set mode + enable */
}

void AnalogCompClass::detachInterrupt() {
  AC0.INTCTRL = 0;
  _callback = nullptr;
}

void AnalogCompClass::_irq() {
  AC0.STATUS = AC_CMP_bm; /* clear flag */
  if (_callback) _callback();
}

ISR(AC0_AC_vect) {
  AnalogComp._irq();
}
