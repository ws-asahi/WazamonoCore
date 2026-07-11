/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Route_logic_pins.ino                                                  |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals on the      |
| megaAVR to create a 3-input AND gate using logic block 0, but we      |
| utilize the event system to route logic input 0 and 1 to pins D8 and  |
| D9 instead of the default pins.                                       |
| Here's how 0x80 turns out to be the correct value to create a 3-input |
| AND gate:                                                             |
|                                           3-input AND truth table:    |
| If we look at the truth table             |PA2|D9 |D8 | Y |           |
| to the right, we can see that             |---|---|---|---|           |
| all binary values for Y can               | 0 | 0 | 0 | 0 |           |
| be represented as 10000000.               | 0 | 0 | 1 | 0 |           |
| If we convert this 8-bit                  | 0 | 1 | 0 | 0 |           |
| binary number into hex, we                | 0 | 1 | 1 | 0 |           |
| get 0x80.                                 | 1 | 0 | 0 | 0 |           |
|                                           | 1 | 0 | 1 | 0 |           |
| In this example the output pin,           | 1 | 1 | 0 | 0 |           |
| PA3 will go high if all three             | 1 | 1 | 1 | 1 |           |
| inputs are high.                                                      |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>
#include <Logic.h>

void setup() {
  // Initialize Event channel 2 and 3
  /* The DU-series has the version 3 event system: pins are routed through the port
   * event generators (PORTx.EVGENCTRLA) by set_generator(pin). Any pin can be an
   * event input; LUT0's direct input 2 (PA2) and output (PA3) are fixed pins. */
  Event2.set_generator((uint8_t)8); // Set pin D8 as event generator
  Event3.set_generator((uint8_t)9); // Set pin D9 as event generator
  Event2.set_user(user::ccl0_event_a); // Set CCL0 (Logic0) event A as user
  Event3.set_user(user::ccl0_event_b); // Set CCL0 (Logic0) event B as user

  // Initialize logic block 0
  Logic0.enable = true;                // Enable logic block 0
  Logic0.input0 = in::event_a;         // Connect input 0 to ccl0_event_a (D8 through Event2)
  Logic0.input1 = in::event_b;         // Connect input 1 to ccl0_event_b (D9 through Event3)
  Logic0.input2 = in::input;           // LUT0 IN2 = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3)
  Logic0.output = out::enable;         // LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.truth = 0x80;                 // Set truth table
  Logic0.init();

  // Start event channels and the logic hardware
  Event2.start();
  Event3.start();
  Logic::start();
}

void loop() {

}
