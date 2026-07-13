/***********************************************************************|
| AVR DU analog comparator library (AC0)                                |
|                                                                       |
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| https://github.com/SpenceKonde/megaTinyCore                           |
| Ported to tinyAVR 2022 by Spence Konde for DxCore                     |
| https://github.com/SpenceKonde/DxCore                                 |
|                                                                       |
| In this example we use an internal reference voltage instead of an    |
| external one on the negative pin. This eliminates the need for an     |
| external voltage divider to generate a reference. Note that the       |
| internal reference requires a stable voltage to function properly.    |
|                                                                       |
| This is the formula for the generated voltage:                        |
| Vdacref = (DACREF / 256) * Vref                                       |
| The DU AC0 references are 1.024, 2.048, 4.096 and 2.5 Volts; you need |
| about half a volt of headroom between VRef and VDD for an accurate    |
| reference voltage.                                                    |
|                                                                       |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;       // Use positive input 0: in0 = PD2 (Tachi: A1 / Tsurugi: D9; not bonded out on Kunai - use in3/in4 there)
  Comparator.input_n = comparator::in_n::dacref;    // Connect the negative pin to the DACREF voltage
  Comparator.reference = comparator::ref::vref_2v5; // Set the DACREF voltage to 2.5V
  Comparator.dacref = 127;              // Gives us 1.24V -> (127 / 256) * 2.5V = 1.24V
  Comparator.hysteresis = comparator::hyst::large;  // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;      // Enable output on AC0 OUT = PA7 (Tachi: D4 / Tsurugi: D8 / Kunai: D0)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
