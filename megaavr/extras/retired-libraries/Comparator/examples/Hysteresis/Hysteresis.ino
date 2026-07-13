/***********************************************************************|
| AVR DU analog comparator library (AC0)                                |
|                                                                       |
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| Ported to tinyAVR & Dx-series by Spence Konde for megaTinyCore and    |
| DxCore 2021-2022: https://github.com/SpenceKonde/                     |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise. We'll also use the built-in    |
| hysteresis functionality to prevent spurious signals while the        |
| voltage on the two pins is very close                                 |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;      // Use positive input 0: in0 = PD2 (Tachi: A1 / Tsurugi: D9; not bonded out on Kunai - use in3/in4 there)
  Comparator.input_n = comparator::in_n::in0;      // Use negative input 0: in0 = PD3 (Tachi: A0 / Tsurugi: D10; not bonded out on Kunai - use in2 = PD7)
  Comparator.hysteresis = comparator::hyst::large; // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;     // Enable output on AC0 OUT = PA7 (Tachi: D4 / Tsurugi: D8 / Kunai: D0)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
