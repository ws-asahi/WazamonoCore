/* AnalogComp.h - Beginner-friendly analog comparator (AC0) for WazamonoCore.
 *
 * Part of WazamonoCore for the AVR DU-series Wazamono boards.
 * (C) 2026 Yusuke Shimizu - Workshop Asahi. LGPL 2.1.
 *
 * A single pre-instantiated object "AnalogComp" wraps AC0, in the same
 * spirit as Serial or SD:
 *
 *   AnalogComp.begin();            // compare plus pin vs minus pin
 *   AnalogComp.begin(2.5);         // compare plus pin vs internal 2.5 V
 *   if (AnalogComp.read()) ...     // true when plus > minus
 *
 * Default input pins (fixed by the AC0 hardware mux):
 *              plus (+)             minus (-)
 *   Tachi      A1  (PD2, AINP0)     A0  (PD3, AINN0)
 *   Tsurugi    D9  (PD2, AINP0)     D10 (PD3, AINN0)
 *   Kunai      D6  (PD6, AINP3)     D7  (PD7, AINN2)
 *
 * The comparator output can be driven onto PA7 with enableOutput()
 * (Tachi: D4 / Tsurugi: D8 / Kunai: D0).
 */
#ifndef ANALOGCOMP_H
#define ANALOGCOMP_H

#include <Arduino.h>

#if !defined(WAZAMONO_BOARD_TACHI) && !defined(WAZAMONO_BOARD_TSURUGI) && !defined(WAZAMONO_BOARD_KUNAI)
  #error "AnalogComp supports Wazamono boards only."
#endif

/* Hysteresis levels for setHysteresis() */
#define AC_HYST_NONE   0
#define AC_HYST_SMALL  1
#define AC_HYST_MEDIUM 2
#define AC_HYST_LARGE  3

class AnalogCompClass {
public:
  /* Start comparing the plus pin against the minus pin. */
  bool begin();

  /* Start comparing the plus pin against an internal reference voltage
   * (0 to 4.08 V, generated from DACREF). Returns false if the requested
   * voltage is out of range. */
  bool begin(double thresholdVolts);

  /* Stop the comparator and release the pins. */
  void end();

  /* Current comparator state: true when plus input > minus input. */
  bool read();

  /* Select other input pins before or after begin(). Valid pins:
   *   plus:  PD2 (Tachi A1 / Tsurugi D9), PD6 (Tachi D1* / Tsurugi D13 / Kunai D6)
   *   minus: PD3 (Tachi A0 / Tsurugi D10), PD0 (Tachi A3 / Tsurugi D5),
   *          PD7 (Tachi D0 / Tsurugi D7 / Kunai D7)
   * (* Tachi D1/D0 are the Serial1 pins - usable when Serial1 is not.)
   * Returns false and leaves the setting unchanged if a pin is not a
   * valid comparator input on this board. */
  bool setInputs(uint8_t plusPin, uint8_t minusPin);

  /* Change the internal reference voltage (also switches the minus input
   * to the internal reference). Returns false if out of range. */
  bool setThreshold(double thresholdVolts);

  /* Add hysteresis to suppress chatter near the crossing point:
   * AC_HYST_NONE / AC_HYST_SMALL (~10mV) / AC_HYST_MEDIUM (~25mV) /
   * AC_HYST_LARGE (~50mV). */
  void setHysteresis(uint8_t level);

  /* Drive the comparator result onto the output pin PA7
   * (Tachi: D4 / Tsurugi: D8 / Kunai: D0). invert=true for active-low. */
  void enableOutput(bool invert = false);
  void disableOutput();

  /* Call a function on comparator edges, like attachInterrupt():
   * mode = RISING (plus crosses above minus), FALLING, or CHANGE. */
  void attachInterrupt(void (*callback)(), uint8_t mode);
  void detachInterrupt();

  /* internal - called from the ISR */
  void _irq();

private:
  void applyPins();
  bool applyThreshold(double volts);
  uint8_t _muxpos_gc = 0;   // set in begin() from defaults
  uint8_t _muxneg_gc = 0;
  uint8_t _plusPin  = 255;
  uint8_t _minusPin = 255;  // 255 = internal reference in use
  bool _useDacref = false;
  bool _running = false;
  void (*_callback)() = nullptr;
};

extern AnalogCompClass AnalogComp;

#endif
