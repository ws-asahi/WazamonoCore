/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Simple_Event.ino                                                      |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use pin D8 as an event generator, and the EVOUTA  |
| (PA2) and EVOUTD-alt (PD7) pins as event users: both will follow      |
| D8's state.                                                           |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>

void setup() {
  /* The DU-series has the version 3 event system: all channels are equal, and pins are
   * routed to a channel through the two port event generators (PORTx.EVGENCTRLA), which
   * set_generator(pin) configures for you.
   * Any pin can be the generator; we use D8 here. The event *output* pins however are
   * fixed by the PORTMUX: EVOUTA = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3) and
   * EVOUTD-alt = PD7 (Tachi: D0 / Tsurugi: D7 / Kunai: D7). Both follow D8's state. */
  Event1.set_generator((uint8_t)8);        // Set pin D8 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event1.set_user(user::evouta_pin_pa2);   // EVOUTA = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3)
  Event1.set_user(user::evoutd_pin_pd7);   // EVOUTD-alt = PD7 (Tachi: D0 / Tsurugi: D7 / Kunai: D7)

  // Start the event channel
  Event1.start();
}

void loop() {

}
