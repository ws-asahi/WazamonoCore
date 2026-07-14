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
  Event2.set_generator((uint8_t)8); // Set pin D8 as event generator
  Event3.set_generator((uint8_t)9); // Set pin D9 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event2.set_user(user::evouta_pin_pa2); // EVOUTA = PA2 (Tachi: D2 / Tsurugi: D18 / Kunai: D3)
  #if defined(WAZAMONO_BOARD_TSURUGI)
  Event3.set_user(user::evoutf_pin_pf2); // EVOUTF = PF2 = A2/D16 (PD7 is the AREF pin on Tsurugi)
  #else
  Event3.set_user(user::evoutd_pin_pd7); // EVOUTD-alt = PD7 (Tachi: D0 / Kunai: D7)
  #endif

  // Start event channels
  Event2.start();
  Event3.start();
}

void loop() {
  // Print info about Event2 and its event user
  print_event_info(Event2);
  print_user_info(user::evouta_pin_pa2);

  // Print info about Event3 and its event user
  print_event_info(Event3);
  #if defined(WAZAMONO_BOARD_TSURUGI)
  print_user_info(user::evoutf_pin_pf2);
  #else
  print_user_info(user::evoutd_pin_pd7);
  #endif

  delay(5000);
}
