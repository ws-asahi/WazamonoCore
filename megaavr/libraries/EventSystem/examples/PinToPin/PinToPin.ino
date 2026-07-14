/* EventSystem / PinToPin
 *
 * The smallest possible use of the event system: one pin's level appears on
 * another pin, carried entirely inside the chip. No wire between them, no
 * code in loop() - and it keeps working while the CPU sleeps.
 *
 * The OUTPUT pin must be one of the board's fixed event-output pins:
 *              from (button)   to (LED + resistor, to GND)
 *   Tachi      D8              D2
 *   Tsurugi    D10             D8
 *   Kunai      D8              D0
 *
 * The source pin is pulled up, so with a button to GND the output is HIGH
 * until you press it.
 */
#include <EventSystem.h>

void setup() {
  Serial.begin(115200);

  #if defined(WAZAMONO_BOARD_TACHI)
  bool ok = EventSystem.connect(8, 2);    // D8 -> D2 (EVOUTA)
  #elif defined(WAZAMONO_BOARD_TSURUGI)
  bool ok = EventSystem.connect(10, 8);   // D10 -> D8 (EVOUTA)
  #elif defined(WAZAMONO_BOARD_KUNAI)
  bool ok = EventSystem.connect(8, 0);    // D8 -> D0 (EVOUTA)
  #else
  #error "This example supports Wazamono boards only."
  #endif

  Serial.println(ok ? F("connected - the output pin now follows the button")
                    : F("connect() failed"));
}

void loop() {
  /* Nothing to do - the connection is pure hardware. */
}
