/***********************************************************************|
| AVR DU analog comparator library (AC0)                                |
|                                                                       |
| Simple_comparator.ino                                                 |
|                                                                       |
| A library for interfacing with the AVR analog comparator.             |
| Developed in 2019-2022 by MCUdude                                     |
| https://github.com/MCUdude/                                           |
| Ported 2021-2022 to DxCore, megaTinyCore by Spence Konde.             |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise.                                |
| This is much like transplanting an "LM339" (only with better specs)   |
| into your AVR.                                                        |
| Pins used (AC0, fixed by hardware):                                   |
|       | MCU pin | Tachi | Tsurugi | Kunai        |                    |
| In_p  | PD2     | A1    | D9      | -  (use in3) |                    |
| In_n  | PD3     | A0    | D10     | -  (use in2) |                    |
| Out   | PA7     | D4    | D8      | D0           |                    |
|                                                                       |
| Note that the Event library can be used to redirect the output.       |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;      // Use positive input 0: in0 = PD2 (Tachi: A1 / Tsurugi: D9; not bonded out on Kunai - use in3/in4 there)
  Comparator.input_n = comparator::in_n::in0;      // Use negative input 0: in0 = PD3 (Tachi: A0 / Tsurugi: D10; not bonded out on Kunai - use in2 = PD7)
  Comparator.output = comparator::out::enable;     // Enable output on AC0 OUT = PA7 (Tachi: D4 / Tsurugi: D8 / Kunai: D0)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
