/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Read_event_settings.ino                                               |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example, we demonstrate the possibilities of reading out what |
| event channel we're working with, what generator is used, and which   |
| channel an event user has been connected to.                          |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>

#define MYSERIAL Serial


// Function to print information about the passed event
void print_event_info(Event &my_event) {
  MYSERIAL.printf("This is event channel no. %d\n", my_event.get_channel_number());
  MYSERIAL.printf("This channel uses generator no. 0x%02x, which you can find in Event.h\n", my_event.get_generator());
}

void print_user_info(user::user_t my_user) {
  // Event::get_user_channel() returns -1 if the user isn't connected to any event generator
  MYSERIAL.printf("User 0x%02x is connected to event channel no. %d\n\n", my_user, Event::get_user_channel_number(my_user));
}

void setup() {
  MYSERIAL.begin(115200); // Initialize hardware serial port

  /* The DU-series has the version 3 event system: pins are routed through the port
   * event generators (PORTx.EVGENCTRLA) by set_generator(pin). Any pin can be a
   * generator; the EVOUT pins are fixed by the PORTMUX. */
  #if defined(WAZAMONO_BOARD_TSURUGI)
  Event2.set_generator((uint8_t)5); // Set pin D5 as event generator (D8 = PA7 is the EVOUTA pin here)
  #else
  Event2.set_generator((uint8_t)8); // Set pin D8 as event generator
  #endif
  Event3.set_generator((uint8_t)9); // Set pin D9 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  /* Event output pins are fixed per board (see the pin-configuration table):
   *   Tachi   : EVOUTA = D2 (PA2),  EVOUTD = D0 (PD7),  EVOUTF = D7 (PF2)
   *   Tsurugi : EVOUTA = D8 (PA7),  EVOUTD = D9 (PD2),  EVOUTF = A2 (PF2)
   *   Kunai   : EVOUTA = D0 (PA7),  EVOUTD = D7 (PD7)
   * On Tsurugi, Event3 demonstrates EVOUTF (its generator D9 is the EVOUTD pin). */
  #if defined(WAZAMONO_BOARD_TACHI)
  Event2.set_user(user::evouta_pin_pa2); // EVOUTA = PA2 = D2
  Event3.set_user(user::evoutd_pin_pd7); // EVOUTD = PD7 = D0
  #elif defined(WAZAMONO_BOARD_TSURUGI)
  Event2.set_user(user::evouta_pin_pa7); // EVOUTA = PA7 = D8
  Event3.set_user(user::evoutf_pin_pf2); // EVOUTF = PF2 = A2
  #elif defined(WAZAMONO_BOARD_KUNAI)
  Event2.set_user(user::evouta_pin_pa7); // EVOUTA = PA7 = D0
  Event3.set_user(user::evoutd_pin_pd7); // EVOUTD = PD7 = D7
  #else
  #error "This example supports Wazamono boards only."
  #endif

  // Start event channels
  Event2.start();
  Event3.start();
}

void loop() {
  // Print info about Event2 and its event user
  print_event_info(Event2);
  #if defined(WAZAMONO_BOARD_TSURUGI) || defined(WAZAMONO_BOARD_KUNAI)
  print_user_info(user::evouta_pin_pa7);
  #else
  print_user_info(user::evouta_pin_pa2);
  #endif

  // Print info about Event3 and its event user
  print_event_info(Event3);
  #if defined(WAZAMONO_BOARD_TSURUGI)
  print_user_info(user::evoutf_pin_pf2);
  #else
  print_user_info(user::evoutd_pin_pd7);
  #endif

  delay(5000);
}
