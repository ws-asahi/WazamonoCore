/* pins_arduino.h - Variant definition for the Wazamono Kunai (AVR32DU20)
 * ---------------------------------------------------------------------------
 * Part of WazamonoCore (a product-specific fork of SpenceKonde/DxCore).
 * DxCore is (C) Spence Konde 2021-2022, open source (LGPL 2.1, see LICENSE.md),
 * based on existing Arduino cores. This variant (C) Workshop Asahi 2026.
 *
 * Board   : Wazamono Kunai  (Seeed XIAO form factor, AVR32DU20, USB-C)
 * MCU     : AVR32DU20  (20-pin VQFN, 32 KB flash / 4 KB SRAM)
 * Clock   : internal OSCHF, 24 MHz, NO external crystal.
 *           -> PA0/PA1 are free GPIO (they are the XTALHF pins on crystal
 *              boards, but Kunai is crystal-less, so no clock guards here).
 *           -> USB CLK_USB (48 MHz) = OSCHF + PLL48M, auto-tuned to the USB SOF.
 *
 *  ===== Pin numbering: Seeed XIAO compatible (NONCANONICAL) =====
 *   D#   MCU   XIAO role / notes
 *   D0   PA7   A0  | AC0 OUT | EVOUTA                       AIN27   (no PWM)
 *   D1   PD5   A1                                           AIN5
 *   D2   PA3   A2  | ~PWM(TCA0 WO3) | CCL LUT0-OUT          AIN23   (tone->TCB1)
 *   D3   PA2   A3  | ~PWM(TCA0 WO2) | USART0 XCK | LUT0-IN2 AIN22
 *   D4   PA0   SDA (I2C, TWI0 ALT3) | Serial2 TX | ~PWM(TCA0 WO0) | LUT0-IN0   (no ADC)
 *   D5   PA1   SCL (I2C, TWI0 ALT3) | Serial2 RX | ~PWM(TCA0 WO1) | LUT0-IN1   (no ADC)
 *   D6   PD6   A6  | Serial1 TX (USART1 ALT2)               AIN6
 *   D7   PD7   A7  | Serial1 RX (USART1 ALT2) | EVOUTD      AIN7
 *   D8   PA6   A8  | SCK  (SPI)                             AIN26   (no PWM)
 *   D9   PA5   A9  | MISO (SPI) | ~PWM(TCA0 WO5)            AIN25
 *   D10  PA4   A10 | MOSI (SPI) | ~PWM(TCA0 WO4)            AIN24
 *   D11..D12      (do not exist - gap)
 *   D13  PD4   LED_BUILTIN (on-board, no pad)
 *   --- not on the XIAO pads (appended so the arrays are complete) ---
 *        PC3   VBUS detect (USB power domain)               AIN31   index 14
 *        PF6   RESET                                        index 15
 *        PF7   UPDI                                         index 16  (== PIN_PF7, highest)
 *
 *  ===== Peripheral routing (set by this variant + boards.txt) =====
 *   TCA0  -> PORTA   (WO0..WO5 = PA0..PA5 = D4,D5,D3,D2,D10,D9) : TCA0_PINS below
 *   millis-> TCB0 (boards.txt -DMILLIS_USE_TIMERB0). tone -> TCB1 (DxCore routes
 *           tone to the free TCB; Tone.cpp toggles the pin in software, so tone
 *           works on ANY pin and never disturbs the 6 TCA0 PWM channels. tone and
 *           Servo both want TCB1, so they are mutually exclusive.)
 *   SPI0  -> default (PA4 MOSI / PA5 MISO / PA6 SCK / PA7 SS) = D10/D9/D8/D0.
 *           Board is SPI host; chip-selects are user GPIO (auto-SS not used).
 *   TWI0  -> ALT3 (PA0 SDA / PA1 SCL) = D4/D5. The chip's DEFAULT TWI mux is
 *           PA2/PA3, so PA0/PA1 are the ALT3 (= PINSWAP_3) position. DxCore's Wire
 *           has NO compile-time default-swap, so wazamono_kunai_init.cpp's
 *           initVariant() sets PORTMUX.TWIROUTEA = ALT3 at boot; a plain
 *           Wire.begin() then talks on PA0/PA1. Wire.swap(3) / Wire.pins(PIN_PA0,
 *           PIN_PA1) select the same position.
 *   USART1-> Serial1, ALT2 (PD6 TX / PD7 RX) = D6/D7. (Only usable USART1 position.)
 *   USART0-> Serial2, DEFAULT (PA0 TX / PA1 RX / PA2 XCK / PA3 XDIR) = D4/D5/D3/D2.
 *           <-- shares PA0/PA1 with the I2C pins, so Serial2 and Wire are mutually
 *               exclusive (use one or the other).
 *   Serial-> native USB CDC (USBSerial), Leonardo/Micro convention.
 *
 *   NOTE on names: in DxCore USART0 is "Serial0" and USART1 is "Serial1". The
 *   on-board hardware UART on the XIAO TX/RX pads (D6/D7) is USART1 = Serial1.
 *   The second UART on D4/D5 (USART0) is exposed to users as Serial2 (ascending
 *   board convention); Serial0 remains as its internal alias.
 */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

/* Informational pinout tags. DU_20PIN_PINOUT records the package; these tags are
 * not gated by the core (no #error path), they just identify the board. The real
 * (port,bit) mapping comes from the tables below, selected by
 * NONCANONICAL_PIN_NUMBERS (our numbering does not follow port order). */
#define DU_20PIN_PINOUT
#define WAZAMONO_KUNAI_PINOUT
#define WAZAMONO_BOARD_KUNAI 1  /* Board identification macro (matches bootloader convention) */
#define NONCANONICAL_PIN_NUMBERS

         /*##  ### #   #  ###
          #   #  #  ##  # #
          ####   #  # # #  ###
          #      #  #  ##     #
          #     ### #   #  # */
/* Digital pin number for each MCU pin (Seeed XIAO layout). */
#define PIN_PA7 (0)   // D0  A0 / AC0 OUT / EVOUTA
#define PIN_PD5 (1)   // D1  A1
#define PIN_PA3 (2)   // D2  A2 / TCA0 WO3 / LUT0-OUT
#define PIN_PA2 (3)   // D3  A3 / TCA0 WO2 / USART0 XCK
#define PIN_PA0 (4)   // D4  SDA (TWI0 ALT3) / USART0 TX / TCA0 WO0
#define PIN_PA1 (5)   // D5  SCL (TWI0 ALT3) / USART0 RX / TCA0 WO1
#define PIN_PD6 (6)   // D6  A6 / Serial1 TX (USART1 ALT2)
#define PIN_PD7 (7)   // D7  A7 / Serial1 RX (USART1 ALT2) / EVOUTD
#define PIN_PA6 (8)   // D8  A8 / SPI SCK
#define PIN_PA5 (9)   // D9  A9 / SPI MISO / TCA0 WO5
#define PIN_PA4 (10)  // D10 A10 / SPI MOSI / TCA0 WO4
//  no  D11..D12               (gap)
#define PIN_PD4 (13)  // D13 LED_BUILTIN (on-board only)
#define PIN_PC3 (14)  // VBUS detect (not on a pad)
#define PIN_PF6 (15)  // RESET
#define PIN_PF7 (16)  // UPDI  (highest index -> sets NUM_DIGITAL_PINS = 17)

         /*##   ##   ###  ###  ###  ###
          #   # #  # #      #  #    #
          ####  ####  ###   #  #     ###
          #   # #  #     #  #  #        #
          ####  #  # ####  ###  ###  # */
#define PINS_COUNT                     (17)  // length of the pin tables (incl. gaps/reserved)
#define NUM_ANALOG_INPUTS              (31)  // highest ADC channel in use is AIN31 (PC3)
// NUM_DIGITAL_PINS / NUM_TOTAL_PINS  -> auto = PIN_PF7 + 1 = 17
// NUM_INTERNALLY_USED_PINS           -> auto = 0 (no crystal; PA0/PA1 are GPIO)

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  (PIN_PD4)   // D13, on-board LED
#endif

#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS      (48)
#endif

         /*   #  ###   ### ####   ###   ###
          ## ## #   # #    #   # #   # #
          # # # ##### #    ####  #   #  ###
          #   # #   # #    # #   #   #     #
          #   # #   #  ### #  #   ###   ## */
/* Explicit maps (NONCANONICAL numbering: arithmetic shortcuts cannot be used). */
#define digitalPinToAnalogInput(p)  ( \
    (p) == PIN_PD5 ?  5 : (p) == PIN_PD6 ?  6 : (p) == PIN_PD7 ?  7 : \
    (p) == PIN_PA2 ? 22 : (p) == PIN_PA3 ? 23 : (p) == PIN_PA4 ? 24 : (p) == PIN_PA5 ? 25 : \
    (p) == PIN_PA6 ? 26 : (p) == PIN_PA7 ? 27 : (p) == PIN_PC3 ? 31 : NOT_A_PIN )

#define analogChannelToDigitalPin(p)  ( \
    (p) ==  5 ? PIN_PD5 : (p) ==  6 ? PIN_PD6 : (p) ==  7 ? PIN_PD7 : \
    (p) == 22 ? PIN_PA2 : (p) == 23 ? PIN_PA3 : (p) == 24 ? PIN_PA4 : (p) == 25 ? PIN_PA5 : \
    (p) == 26 ? PIN_PA6 : (p) == 27 ? PIN_PA7 : (p) == 31 ? PIN_PC3 : NOT_A_PIN )

#define analogInputToDigitalPin(p)        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7f) : (((p) <= NUM_DIGITAL_PINS) ? (p) : NOT_A_PIN))
#define portToPinZero(port)               ((port) == PA ? PIN_PA0 : ((port) == PC ? PIN_PC3 : ((port) == PD ? PIN_PD4 : ((port) == PF ? PIN_PF6 : NOT_A_PIN))))

/* PWM ---------------------------------------------------------------------- */
/* All 6 PWM channels are TCA0 (routed to PORTA below). tone() uses TCB1 in
 * software (any pin), so no pin uses a TCB hardware output -> no TCB PWM here. */
#define digitalPinHasPWMTCB(p)          (0)

/* TCA0 is routed to PORTA, so WO0..WO5 land on PA0..PA5. In XIAO numbering those
 * pins are D4,D5,D3,D2,D10,D9 (NOT contiguous), so the PWM test is an explicit
 * OR-list rather than a range. */
#define TCA0_PINS                       (PORTMUX_TCA0_PORTA_gc)
#define TCB0_PINS                       (0x00)   // TCB0 = millis (no user WO pin)
#define TCB1_PINS                       (0x00)   // TCB1 = tone   (software, any pin)

#define PIN_TCA0_WO0_INIT               (PIN_PA0)
#define PIN_TCB0_WO_INIT                (PIN_PA2)
#define PIN_TCB1_WO_INIT                (PIN_PA3)

#define digitalPinHasPWM(p)             ( (p) == PIN_PA0 || (p) == PIN_PA1 || (p) == PIN_PA2 || \
                                          (p) == PIN_PA3 || (p) == PIN_PA4 || (p) == PIN_PA5 )

         /*##   ###  ####  ##### #   # #   # #   #
          #   # #   # #   #   #   ## ## #   #  # #
          ####  #   # ####    #   # # # #   #   #
          #     #   # #  #    #   #   # #   #  # #
          #      ###  #   #   #   #   #  ###  #   */
#define SPI_INTERFACES_COUNT            (1)

// SPI 0  (host; chip-selects are user GPIO)
#define SPI_MUX                         (PORTMUX_SPI0_DEFAULT_gc)
#define SPI_MUX_PINSWAP_NONE            (PORTMUX_SPI0_NONE_gc)
#define PIN_SPI_MOSI                    (PIN_PA4)   // D10
#define PIN_SPI_MISO                    (PIN_PA5)   // D9
#define PIN_SPI_SCK                     (PIN_PA6)   // D8
#define PIN_SPI_SS                      (PIN_PA7)   // D0 (hardware SS; shared with AC0 OUT)

// TWI 0  (I2C on D4/D5 = PA0/PA1 = TWI0 ALT3; see header + wazamono_kunai_init.cpp)
//   PIN_WIRE_SDA/SCL must name the chip's DEFAULT-mux pins (PA2/PA3) because the
//   core equates "PIN_WIRE_SDA" with PORTMUX_TWI0_DEFAULT_gc. PA0/PA1 are exposed
//   as PINSWAP_3 (ALT3) and made the power-on default in initVariant().
#define PIN_WIRE_SDA                    (PIN_PA2)   // TWI0 DEFAULT-mux home (D3); not the I2C pad
#define PIN_WIRE_SCL                    (PIN_PA3)   // TWI0 DEFAULT-mux home (D2); not the I2C pad
#define PIN_WIRE_SDA_PINSWAP_1          (NOT_A_PIN)
#define PIN_WIRE_SCL_PINSWAP_1          (NOT_A_PIN)
#define PIN_WIRE_SDA_PINSWAP_3          (PIN_PA0)   // ALT3 = PA0 = D4 (Kunai I2C SDA, power-on default)
#define PIN_WIRE_SCL_PINSWAP_3          (PIN_PA1)   // ALT3 = PA1 = D5 (Kunai I2C SCL, power-on default)

// USART0 -> "Serial0", user-facing "Serial2". DEFAULT position (PA0/PA1 = D4/D5).
#define HWSERIAL0_MUX                   (0x00 /* PORTMUX_USART0_DEFAULT_gc */)
#define HWSERIAL0_MUX_PINSWAP_1         (0x01 /* PORTMUX_USART0_ALT1_gc - PA4..PA7 */)
#define HWSERIAL0_MUX_PINSWAP_2         (0x02 /* PORTMUX_USART0_ALT2_gc - PA2/PA3 */)
#define HWSERIAL0_MUX_PINSWAP_3         (0x03 /* PORTMUX_USART0_ALT3_gc - PD4..PD7 */)
#define HWSERIAL0_MUX_PINSWAP_NONE      (0x05)
#define HWSERIAL0_MUX_DEFAULT          (0)   /* Kunai default: USART0 DEFAULT (PA0/PA1 = D4/D5). */
#define PIN_HWSERIAL0_TX                (PIN_PA0)   // D4
#define PIN_HWSERIAL0_RX                (PIN_PA1)   // D5
#define PIN_HWSERIAL0_XCK               (PIN_PA2)   // D3
#define PIN_HWSERIAL0_XDIR              (PIN_PA3)   // D2
#define PIN_HWSERIAL0_TX_PINSWAP_1      (PIN_PA4)
#define PIN_HWSERIAL0_RX_PINSWAP_1      (PIN_PA5)
#define PIN_HWSERIAL0_XCK_PINSWAP_1     (PIN_PA6)
#define PIN_HWSERIAL0_XDIR_PINSWAP_1    (PIN_PA7)
#define PIN_HWSERIAL0_TX_PINSWAP_2      (PIN_PA2)
#define PIN_HWSERIAL0_RX_PINSWAP_2      (PIN_PA3)
#define PIN_HWSERIAL0_XCK_PINSWAP_2     (NOT_A_PIN)
#define PIN_HWSERIAL0_XDIR_PINSWAP_2    (NOT_A_PIN)
#define PIN_HWSERIAL0_TX_PINSWAP_3      (PIN_PD4)
#define PIN_HWSERIAL0_RX_PINSWAP_3      (PIN_PD5)
#define PIN_HWSERIAL0_XCK_PINSWAP_3     (PIN_PD6)
#define PIN_HWSERIAL0_XDIR_PINSWAP_3    (PIN_PD7)

// USART1 -> "Serial1", only usable position is ALT2 (PD6/PD7 = D6/D7)
#define HWSERIAL1_MUX                   (0x00 /* PORTMUX_USART1_DEFAULT_gc - no pins */)
#define HWSERIAL1_MUX_PINSWAP_1         (0x01 << 3 /* ALT1 absent on DU - placeholder so the PINSWAP_2 row is built */)
#define HWSERIAL1_MUX_PINSWAP_2         (0x02 << 3 /* PORTMUX_USART1_ALT2_gc */)
#define HWSERIAL1_MUX_PINSWAP_NONE      (0x03 << 3)
#define PIN_HWSERIAL1_TX                (NOT_A_PIN)
#define PIN_HWSERIAL1_RX                (NOT_A_PIN)
#define PIN_HWSERIAL1_XCK               (NOT_A_PIN)
#define PIN_HWSERIAL1_XDIR              (NOT_A_PIN)
#define PIN_HWSERIAL1_TX_PINSWAP_1      (NOT_A_PIN)
#define PIN_HWSERIAL1_RX_PINSWAP_1      (NOT_A_PIN)
#define PIN_HWSERIAL1_XCK_PINSWAP_1     (NOT_A_PIN)
#define PIN_HWSERIAL1_XDIR_PINSWAP_1    (NOT_A_PIN)
#define PIN_HWSERIAL1_TX_PINSWAP_2      (PIN_PD6)   // D6
#define PIN_HWSERIAL1_RX_PINSWAP_2      (PIN_PD7)   // D7
#define PIN_HWSERIAL1_XCK_PINSWAP_2     (NOT_A_PIN)
#define PIN_HWSERIAL1_XDIR_PINSWAP_2    (NOT_A_PIN)
#define HWSERIAL1_MUX_DEFAULT          (2)   /* DU: USART1 ALT2 (PD6/PD7); USART1 has no usable DEFAULT position */

         /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
          #   # ##  # #   # #    #   # #         #   #  #  ##  # #
          ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
          #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
          #   # #   # #   # ####  ###   ###      #     ### #   #  # */
/* Arduino analog aliases. XIAO keeps A0..A3 and A6..A10 (one per pad); A4/A5 do
 * not exist because their pads (D4=PA0, D5=PA1) have no ADC input on the DU. */
#define PIN_A0   (PIN_PA7)   // D0
#define PIN_A1   (PIN_PD5)   // D1
#define PIN_A2   (PIN_PA3)   // D2
#define PIN_A3   (PIN_PA2)   // D3
#define PIN_A4   (NOT_A_PIN)
#define PIN_A5   (NOT_A_PIN)
#define PIN_A6   (PIN_PD6)   // D6
#define PIN_A7   (PIN_PD7)   // D7
#define PIN_A8   (PIN_PA6)   // D8
#define PIN_A9   (PIN_PA5)   // D9
#define PIN_A10  (PIN_PA4)   // D10
/* PD4 (D13, LED_BUILTIN) and PC3 (VBUS detect) get no Ax alias: PD4 has no ADC,
 * PC3 is the VBUS-sense pin (still reachable as analogRead(AIN31) if needed). */

/* --- Uno R4 style number-prefixed digital pin aliases (pads + LED only) ---
 * D-number == Arduino digital pin number. Internal-only pins (PC3 VBUS-detect,
 * PF6 RESET, PF7 UPDI) are intentionally NOT exposed as Dn.
 * #undef guards clear any stray macro definitions, matching the Uno R4 pattern. */
#undef D0
#undef D1
#undef D2
#undef D3
#undef D4
#undef D5
#undef D6
#undef D7
#undef D8
#undef D9
#undef D10
#undef D13
static const uint8_t D0  = PIN_PA7;  // A0 / AC0 OUT
static const uint8_t D1  = PIN_PD5;  // A1
static const uint8_t D2  = PIN_PA3;  // A2 / TCA0 WO3
static const uint8_t D3  = PIN_PA2;  // A3 / TCA0 WO2 / USART0 XCK
static const uint8_t D4  = PIN_PA0;  // SDA / Serial2 TX / TCA0 WO0
static const uint8_t D5  = PIN_PA1;  // SCL / Serial2 RX / TCA0 WO1
static const uint8_t D6  = PIN_PD6;  // Serial1 TX
static const uint8_t D7  = PIN_PD7;  // Serial1 RX
static const uint8_t D8  = PIN_PA6;  // SCK
static const uint8_t D9  = PIN_PA5;  // MISO / TCA0 WO5
static const uint8_t D10 = PIN_PA4;  // MOSI / TCA0 WO4
static const uint8_t D13 = PIN_PD4;  // LED_BUILTIN (on-board only, no pad)

#undef A0
#undef A1
#undef A2
#undef A3
#undef A6
#undef A7
#undef A8
#undef A9
#undef A10
static const uint8_t A0   = PIN_A0;
static const uint8_t A1   = PIN_A1;
static const uint8_t A2   = PIN_A2;
static const uint8_t A3   = PIN_A3;
static const uint8_t A6   = PIN_A6;
static const uint8_t A7   = PIN_A7;
static const uint8_t A8   = PIN_A8;
static const uint8_t A9   = PIN_A9;
static const uint8_t A10  = PIN_A10;

/* Direct ADC channel identifiers (ADC_CH() sets the 0x80 "this is a channel" flag). */
#define AIN5   ADC_CH(5)
#define AIN6   ADC_CH(6)
#define AIN7   ADC_CH(7)
#define AIN22  ADC_CH(22)
#define AIN23  ADC_CH(23)
#define AIN24  ADC_CH(24)
#define AIN25  ADC_CH(25)
#define AIN26  ADC_CH(26)
#define AIN27  ADC_CH(27)
#define AIN31  ADC_CH(31)

         /*##  ### #   #      ###  ####  ####   ###  #   #  ###
          #   #  #  ##  #     #   # #   # #   # #   #  # #  #
          ####   #  # # #     ##### ####  ####  #####   #    ###
          #      #  #  ##     #   # #  #  #  #  #   #   #       #
          #     ### #   #     #   # #   # #   # #   #   #    # */
#ifdef ARDUINO_MAIN
  // Indexed by digital pin number (0..16). Gaps and reserved pins are NOT_A_PORT.
  const uint8_t digital_pin_to_port[] = {
    PA,         //  0 PA7  A0 / AC0 OUT
    PD,         //  1 PD5  A1
    PA,         //  2 PA3  A2 / TCA0 WO3
    PA,         //  3 PA2  A3 / TCA0 WO2
    PA,         //  4 PA0  SDA / Serial2 TX / TCA0 WO0
    PA,         //  5 PA1  SCL / Serial2 RX / TCA0 WO1
    PD,         //  6 PD6  Serial1 TX
    PD,         //  7 PD7  Serial1 RX
    PA,         //  8 PA6  SCK
    PA,         //  9 PA5  MISO / TCA0 WO5
    PA,         // 10 PA4  MOSI / TCA0 WO4
    NOT_A_PORT, // 11 (gap)
    NOT_A_PORT, // 12 (gap)
    PD,         // 13 PD4  LED_BUILTIN
    PC,         // 14 PC3  VBUS detect
    PF,         // 15 PF6  RESET
    PF          // 16 PF7  UPDI
  };

  /* Bit position within the port (for PINnCTRL access). */
  const uint8_t digital_pin_to_bit_position[] = {
    PIN7_bp,   //  0 PA7
    PIN5_bp,   //  1 PD5
    PIN3_bp,   //  2 PA3
    PIN2_bp,   //  3 PA2
    PIN0_bp,   //  4 PA0
    PIN1_bp,   //  5 PA1
    PIN6_bp,   //  6 PD6
    PIN7_bp,   //  7 PD7
    PIN6_bp,   //  8 PA6
    PIN5_bp,   //  9 PA5
    PIN4_bp,   // 10 PA4
    NOT_A_PIN, // 11 (gap)
    NOT_A_PIN, // 12 (gap)
    PIN4_bp,   // 13 PD4
    PIN3_bp,   // 14 PC3
    PIN6_bp,   // 15 PF6
    PIN7_bp    // 16 PF7
  };

  const uint8_t digital_pin_to_bit_mask[] = {
    PIN7_bm,   //  0 PA7
    PIN5_bm,   //  1 PD5
    PIN3_bm,   //  2 PA3
    PIN2_bm,   //  3 PA2
    PIN0_bm,   //  4 PA0
    PIN1_bm,   //  5 PA1
    PIN6_bm,   //  6 PD6
    PIN7_bm,   //  7 PD7
    PIN6_bm,   //  8 PA6
    PIN5_bm,   //  9 PA5
    PIN4_bm,   // 10 PA4
    NOT_A_PIN, // 11 (gap)
    NOT_A_PIN, // 12 (gap)
    PIN4_bm,   // 13 PD4
    PIN3_bm,   // 14 PC3
    PIN6_bm,   // 15 PF6
    PIN7_bm    // 16 PF7
  };

  /* All PWM is TCA0, resolved dynamically from PORTMUX, so every TCA0 pin is
   * NOT_ON_TIMER here (analogWrite resolves them via TCA0_PINS). millis uses TCB0
   * and tone uses TCB1 in software, neither of which is a user PWM output, so no
   * pin is mapped to a TCB. */
  const uint8_t digital_pin_to_timer[] = {
    NOT_ON_TIMER, //  0 PA7
    NOT_ON_TIMER, //  1 PD5
    NOT_ON_TIMER, //  2 PA3  (TCA0 WO3, dynamic)
    NOT_ON_TIMER, //  3 PA2  (TCA0 WO2, dynamic)
    NOT_ON_TIMER, //  4 PA0  (TCA0 WO0, dynamic)
    NOT_ON_TIMER, //  5 PA1  (TCA0 WO1, dynamic)
    NOT_ON_TIMER, //  6 PD6
    NOT_ON_TIMER, //  7 PD7
    NOT_ON_TIMER, //  8 PA6
    NOT_ON_TIMER, //  9 PA5  (TCA0 WO5, dynamic)
    NOT_ON_TIMER, // 10 PA4  (TCA0 WO4, dynamic)
    NOT_ON_TIMER, // 11 (gap)
    NOT_ON_TIMER, // 12 (gap)
    NOT_ON_TIMER, // 13 PD4
    NOT_ON_TIMER, // 14 PC3
    NOT_ON_TIMER, // 15 PF6
    NOT_ON_TIMER  // 16 PF7
  };
#endif

/* =================================================================
 *  USB identity   (AVR DU = USB-native part, treated like the 32U4)
 * =================================================================
 *  USBCON enables Arduino's HID / Keyboard / Mouse / etc. on this board.
 *  NOTE: the native USB-CDC descriptor's VID/PID/product string are taken
 *  from cores/dxcore/usb_descriptors.{h,c} (which does NOT include this
 *  file), so the values below are informational only - keep them in sync.
 *  Effective app identity: 0x1209:0x000A, product "Wazamono Kunai".
 *  Obtain a real product VID/PID before release (pid.codes = dev only).
 */
#ifndef USBCON
  #define USBCON
#endif
#ifndef USB_VID
  #define USB_VID                0x1209
#endif
#ifndef USB_PID
  #define USB_PID                0x000A
#endif
#ifndef USB_MANUFACTURER
  #define USB_MANUFACTURER       "Workshop Asahi"
#endif
#ifndef USB_PRODUCT
  #define USB_PRODUCT            "Wazamono Kunai"
#endif

/* =================================================================
 *  Serial -> native USB CDC   (Leonardo/Micro convention)
 * =================================================================
 *  Serial  = USBSerial (on-chip USB CDC)              <- primary / USB serial monitor
 *  Serial1 = USART1   (D6/D7, ALT2 - the on-board hardware UART pads)
 *  Serial2 = USART0   (D4/D5, DEFAULT - extra UART; shares pins with Wire/I2C,
 *                      so Serial2 and Wire are mutually exclusive)
 *  Serial0 is the DxCore-internal name for USART0; the user-facing name is Serial2.
 *  Define HAVE_NO_USB_SERIAL_REDIRECT (from boards.txt) to keep Serial==USART0.
 */

/* Expose USART0 as Serial2 (ascending board convention). The core's Serial0
 * object is kept intact for upstream mergeability; this is only a user-facing
 * alias. Serial2 (USART2) does not exist on the AVR DU, so the name is free. */
#ifndef Serial2
  #define Serial2 Serial0
#endif

#if defined(USB0) && !defined(HAVE_NO_USB_SERIAL_REDIRECT)
  #ifndef Serial
    #define Serial                  USBSerial   /* Serial = native USB CDC   */
  #endif
  #ifndef SERIAL_PORT_MONITOR
    #define SERIAL_PORT_MONITOR     Serial
  #endif
  #ifndef SERIAL_PORT_USBVIRTUAL
    #define SERIAL_PORT_USBVIRTUAL  Serial
  #endif
  #ifndef SERIAL_PORT_HARDWARE
    #define SERIAL_PORT_HARDWARE    Serial1     /* on-board hardware UART on D6/D7 */
  #endif
  #ifndef SERIAL_PORT_HARDWARE_OPEN
    #define SERIAL_PORT_HARDWARE_OPEN  Serial2  /* extra UART on D4/D5 (USART0) */
  #endif
#endif

#endif /* Pins_Arduino_h */
