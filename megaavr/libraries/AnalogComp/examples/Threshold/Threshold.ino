/* AnalogComp / Threshold
 *
 * Compares the plus input pin against an internal reference voltage -
 * no external divider needed. Here the threshold is 2.5 V: the built-in
 * LED turns on while the plus pin is above it.
 *
 * Plus input pin: Tachi A1 / Tsurugi D9 / Kunai D6
 * The reference is generated from DACREF; any value from 0 to 4.08 V works.
 */
#include <AnalogComp.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  AnalogComp.begin(2.5);                    // plus pin vs internal 2.5 V
  AnalogComp.setHysteresis(AC_HYST_MEDIUM);
}

void loop() {
  // Note: LED_BUILTIN is active-LOW on Tachi, so "on" and "off" are swapped there.
  digitalWrite(LED_BUILTIN, AnalogComp.read() ? HIGH : LOW);
  delay(10);
}
