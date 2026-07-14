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
   * set_generator(pin) configures for you - any pin can be the generator.
   * The event OUTPUT pins are fixed per board (see the board's pin-configuration
   * table); both outputs below follow the generator pin's state:
   *   Tachi   : generator D8,  EVOUTA = D2 (PA2),  EVOUTD = D0 (PD7)
   *   Tsurugi : generator D10, EVOUTA = D8 (PA7),  EVOUTD = D9 (PD2)
   *             (D8/D9 are this board's event-output pins, so the
   *             generator moves to D10)
   *   Kunai   : generator D8,  EVOUTA = D0 (PA7),  EVOUTD = D7 (PD7) */
  #if defined(WAZAMONO_BOARD_TSURUGI)
  Event1.set_generator((uint8_t)10);       // Set pin D10 as event generator
  #else
  Event1.set_generator((uint8_t)8);        // Set pin D8 as event generator
  #endif

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  #if defined(WAZAMONO_BOARD_TACHI)
  Event1.set_user(user::evouta_pin_pa2);   // EVOUTA = PA2 = D2 (also I2C SDA)
  Event1.set_user(user::evoutd_pin_pd7);   // EVOUTD = PD7 = D0 (also Serial1 RX)
  #elif defined(WAZAMONO_BOARD_TSURUGI)
  Event1.set_user(user::evouta_pin_pa7);   // EVOUTA = PA7 = D8
  Event1.set_user(user::evoutd_pin_pd2);   // EVOUTD = PD2 = D9
  #elif defined(WAZAMONO_BOARD_KUNAI)
  Event1.set_user(user::evouta_pin_pa7);   // EVOUTA = PA7 = D0 (also AC0 OUT / SPI SS)
  Event1.set_user(user::evoutd_pin_pd7);   // EVOUTD = PD7 = D7 (also Serial1 RX)
  #else
  #error "This example supports Wazamono boards only."
  #endif

  // Start the event channel
  Event1.start();
}

void loop() {

}
