/***********************************************************************|
| Configurable Custom Logic library                                     |
| Developed in 2019 by MCUdude. https://github.com/MCUdude/             |
| Ported to DxC and maintained for mTC and DxC by Sprence Konde         |
| Example by Spence Konde 2021                                          |
| Modified to use event system and work on DD14 by Andrew J. Kroll      |
|                                                                       |
| LatchNoSeqDx.ino - Getting "RS Latch" like behavior with a single   |
| even numbered LUT. Should work on ALL parts.                          |
|                                                                       |
| See the original LatchNoSeq for additional discussion of the rationale|
| This version is part of WazamonoCore for the AVR DU-series.           |
| It demonstrates use of event channels to deal with pin constraints    |
|                                                                       |
| In this example we use the configurable logic peripherals in AVR      |
| to act as a "latch" WITHOUT using both LUTs and the sequencer         |
| For the even-numbered logic block(s) we can simply use the feedback   |
| input. Otherwise we need to use the event system.                     |
|                                                                       |
|                                           3-input     truth table:    |
| We use CCL LUT event as our "feedback",   |D8 |D9 |CCL| Y |           |
| D8 is RESET and D9 is SET, both           |---|---|---|---|           |
| active low                                | 0 | 0 | 0 | 0 |           |
| Connect a button between those and Gnd    | 0 | 0 | 1 | 1 |           |
| Pressing button on D9 will set output     | 0 | 1 | 0 | 1 |           |
| HIGH and pressing button on D8 will set   | 0 | 1 | 1 | 1 |           |
| output LOW, and pressing neither will do  | 1 | 0 | 0 | 0 |           |
| nothing.                                  | 1 | 0 | 1 | 0 |           |
| We could even then fire an interrupt from | 1 | 1 | 0 | 0 |           |
| that pin                                  | 1 | 1 | 1 | 1 |           |
|                                                                       |
| The sky (well, and the number of LUTs) is the limit!!                 |
|                                                                       |
| This file is part of WazamonoCore.                                    |
|***********************************************************************/


// D8 RESET, D9 SET -> PD7 RESULT (Tachi: D0 / Tsurugi: D7 / Kunai: D7)
#include <Logic.h>
#include <Event.h>

void setup() {

  // low for reset
  pinMode(8, INPUT_PULLUP);          // D8
  Event2.set_generator(8);
  Event2.set_user(user::ccl0_event_a);
  Event2.start();

  // low for set
  pinMode(9, INPUT_PULLUP);          // D9
  Event3.set_generator(9);
  Event3.set_user(user::ccl0_event_b);
  Event3.start();
  Event4.set_generator(gen::ccl0_out);
  Event4.set_user(user::evoutd_pin_pd7); // result on PD7 (Tachi: D0 / Tsurugi: D7 / Kunai: D7)
  Event4.start();

  Logic0.enable = true;
  Logic0.input0 = in::feedback;
  Logic0.input1 = in::event_a;            // D8 as input1 (RESET)
  Logic0.input2 = in::event_b;            // D9 as input2 (SET)
  Logic0.filter = filter::disable;        // No output filter enabled
  Logic0.truth = 0x8E;                    // Set truth table
  Logic0.init();                          // Initialize logic block 0
  Logic::start();

}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything
}
