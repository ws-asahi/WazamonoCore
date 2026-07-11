/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0xFF (blank).
 * Please see eeprom_iteration for a more in depth
 * look at how to traverse the EEPROM.
 *
 * This example code is in the public domain.
 */

#include <EEPROM.h>

void setup() {
  // initialize the LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  /*
   * Iterate through each byte of the EEPROM storage.

   * Larger AVR processors have larger EEPROM sizes, E.g:
   * The AVR DU-series (all Wazamono boards) has 256 bytes of EEPROM,
   * regardless of flash size.

   * Rather than hard-coding the length, you should use the pre-provided length function.
   * This will make your code portable to all AVR processors.
   */

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0xFF);
  }

  // turn the LED on when we're done (note: LED_BUILTIN is active-LOW on Tachi)
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  /* Empty loop.  */
}
