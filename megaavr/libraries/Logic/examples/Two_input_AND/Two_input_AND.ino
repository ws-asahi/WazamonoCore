/***********************************************************************|
| AVR DU Configurable Custom Logic library                              |
|                                                                       |
| Two_input_AND.ino                                                     |
|                                                                       |
| A library for interfacing with the AVR DU Configurable Custom Logic.  |
| Developed in 2019 by MCUdude. Example fixed Spence Konde 2021         |
| This version is part of WazamonoCore for the AVR DU-series.           |
|                                                                       |
| In this example we use the configurable logic peripherals of the      |
| AVR DU to create a 2-input AND gate using logic block 0 on PORT A.   |
| The example is pretty straight forward, but the truth table value may |
| be a little difficult to understand at first glance.                  |
| We will only use PA1 and PA2 as inputs. when the first input is       |
| disabled it will always be read as 0, and the output doesn't matter.  |
| Here's how 0x40 turns out to be the correct value to create a 2-input |
| NAND gate (or one of them rather - with 4 don't-care bits, there are  |
| 16 that will all give the same behavior)                              |
|                                     2-input AND truth table:          |
| If we look at the truth table       |IN2|IN1|IN0| Y |                 |
| to the right, we can see that       |PA2|PA1|---|OUT|                 |
| all binary values for Y can         |---|---|---|---|                 |
| be represented as x1x0x0x0.         | 0 | 0 | 0 | 0 |                 |
| Using a 0 for all don't care bits   | 0 | 0 | 1 | x | IN0 is always 0 |
| gives us 0b01000000 or 0x40 in hex  | 0 | 1 | 0 | 0 |                 |
| 0b11000000 is just as valid and     | 0 | 1 | 1 | x | IN0 is always 0 |
| might be more "obvious" at a glance | 1 | 0 | 0 | 0 |                 |
| so 0xC0 is just as good. However    | 1 | 0 | 1 | x | IN0 is always 0 |
| it is harder to defend choices like | 1 | 1 | 0 | 1 |                 |
| 0b01100010 - same behavior, but     | 1 | 1 | 1 | x | IN0 is always 0 |
| much more confusing, because the reader will immediately assume that  |
| those 1's to the right mean something - when in fact they're never    |
| used if input0 is masked!                                             |
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
  // It has one output, PA3, with alternate output on PA6
  Logic0.enable = true;                 // Enable logic block 0
  Logic0.input1 = in::input_pullup;     // IN1 = PA1 (Kunai: D5; crystal on Tachi/Tsurugi)
  Logic0.input2 = in::input_pullup;     // IN2 = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3)
  // Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to alternate location
  Logic0.output = out::enable;          // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = filter::disable;      // No output filter enabled
  Logic0.truth = 0xC0;                  // Set truth table

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
