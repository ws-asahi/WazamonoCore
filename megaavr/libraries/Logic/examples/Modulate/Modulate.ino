/***********************************************************************|
| AVR DU Configurable Custom Logic library                              |
| Modulate.ino                                                          |
|                                                                       |
| A library for interfacing with the AVR DU Configurable Custom Logic.  |
| Developed in 2019 by MCUdude.       Example by Spence Konde 2020-2021 |
| https://github.com/MCUdude/            https://github.com/SpenceKonde |
|                                                                       |
| In this example we use the configurable logic peripherals of the of a |
| modern AVR to achieve the "modulate one timer's PWN with another's"   |
| behavior like some of the more full-featured classic megaAVR parts    |
| We set one input to TCA0 WO1, the other to TCB1 WO, and mask the third|
| and set the truth table so that the output is only HIGH when both are |
| HIGH. You can add some third condition as well, if that was somehow   |
| useful too!                                                           |
| As can be immediately seen, this is vastly more powerful than what the|
| classic AVRs were capable of, where this could be done only with two  |
| timers on a single pin. Now any CCL output pin can be used be made to |
| output any timer channel, modulated by any other. We could even use   |
| the CCL as an event generator to move the output to one of the EVOUT  |
| pins!                                                                 |
| On the DU, the CCL logic inputs can come from TCB0 or TCB1, but this  |
| is **set by input number** (input0 = TCB0, input1 = TCB1); there is   |
| no in::tcb1 option.                                                   |
|                                                                       |
| In the crude example below the carrier timer is configured with       |
| analogWrite(), so PORTMUX must be pointed at the port that            |
| analogWrite() is used with, AND the pin used must be valid on the     |
| board - hence the per-board block below.                              |
************************************************************************/
/* The carrier comes out of TCA0 WO2, so the physical pin follows each
 * board's PWM port mux. */
#if defined(WAZAMONO_BOARD_TACHI)
  #define NEWMUX PORTMUX_TCA0_PORTF_gc
  #define PIN_TCA_WO2 7                   // D7 (PF2, TCA0 WO2)
#elif defined(WAZAMONO_BOARD_TSURUGI)
  #define NEWMUX PORTMUX_TCA0_PORTD_gc
  #define PIN_TCA_WO2 9                   // D9 (PD2, TCA0 WO2)
#elif defined(WAZAMONO_BOARD_KUNAI)
  #define NEWMUX PORTMUX_TCA0_PORTA_gc
  #define PIN_TCA_WO2 3                   // D3 (PA2, TCA0 WO2)
#else
  #error "This example supports Wazamono boards only."
#endif

#include <Logic.h>

void setup() {
  PORTMUX.TCAROUTEA = NEWMUX;                   // Force TCA onto a known set of pins.

  Logic0.enable = true;                       // Enable logic block 0
  Logic0.input0 = logic::in::tcb;             // TCB0 WO (input0 selects TCB0 on the DU)
  //                                              On those, it's because there's a logic::in::tcb1 option too...
  Logic0.input1 = logic::in::masked;          // Mask input 2
  Logic0.input2 = logic::in::tca0;            // Use TCA0 WO2 as input2
  Logic0.output = logic::out::enable;         // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::disable;     // No output filter enabled
  Logic0.truth = 0x20;                        // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

  analogWrite(PIN_TCA_WO2, 128); // start TCA0 WO2 running
  TCB0.CTRLA = 0x01; // enabled with CLKPER as clock source
  TCB0.CTRLB = 0x07; // PWM8 mode, but output pin not enabled
  TCB0.CCMPL = 255; // 255 counts
  TCB0.CCMPH = 128; // 50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
