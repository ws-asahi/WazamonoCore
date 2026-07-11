/***********************************************************************|
| AVR DU Configurable Custom Logic library                              |
|                                                                       |
| Three_input_OR.ino                                                    |
|                                                                       |
| A library for interfacing with the AVR DU Configurable Custom Logic.  |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals of the      |
| AVR DU to create a 3-input OR gate using logic block 0 on PORT A.    |
| The example is pretty straight forward, but the truth table value may |
| be a little difficult to understand at first glance.                  |
| Here's how 0xFE turns out to be the correct value to create a 3-input |
| OR gate:                                                              |
|                                           3-input OR truth table:     |
| If we look at the truth table             |PA2|PA1|PA0| Y |           |
| to the right, we can see that             |---|---|---|---|           |
| all binary values for Y can               | 0 | 0 | 0 | 0 |           |
| be represented as 11111110.               | 0 | 0 | 1 | 1 |           |
| If we convert this 8-bit                  | 0 | 1 | 0 | 1 |           |
| binary number into hex, we                | 0 | 1 | 1 | 1 |           |
| get 0xFE.                                 | 1 | 0 | 0 | 1 |           |
|                                           | 1 | 0 | 1 | 1 |           |
| In this example the output pin            | 1 | 1 | 0 | 1 |           |
| will go high if one or more               | 1 | 1 | 1 | 1 |           |
| inputs are high.                                                      |
|***********************************************************************/

/* Wazamono pin note: LUT0's direct inputs are fixed pins IN0=PA0, IN1=PA1,
 * IN2=PA2. On Tachi and Tsurugi, PA0/PA1 carry the 24 MHz crystal, so only
 * IN2 (Tachi: D2 / Tsurugi: D18) can be wired directly there - route the
 * other inputs through the event system instead (see Route_logic_pins in
 * the Event library). On Kunai all three are available: IN0 = D4,
 * IN1 = D5, IN2 = D3. The LUT0 output is PA3 (Tachi: D3 / Tsurugi: D19 /
 * Kunai: D2); alternate output PA6 (Tachi: D15 / Tsurugi: D2 / Kunai: D8). */

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // Because PA0 is shared with the UPDI pin and is not usually an option
  // The LUT0 output is PA3, with alternate output on PA6.

  Logic0.enable = true;               // Enable logic block 0

  Logic0.input0 = in::input_pullup;   // IN0 = PA0 (Kunai: D4; crystal on Tachi/Tsurugi)
  Logic0.input1 = in::input_pullup;   // IN1 = PA1 (Kunai: D5; crystal on Tachi/Tsurugi)
  Logic0.input2 = in::input_pullup;   // IN2 = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3)
  // Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to alternate location, if available.
  Logic0.output = out::enable;        // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0xFE;                // Set truth table

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
