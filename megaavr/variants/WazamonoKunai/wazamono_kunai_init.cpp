/* wazamono_kunai_init.cpp - board-specific init for Wazamono Kunai
 * ---------------------------------------------------------------------------
 * Part of WazamonoCore (a product-specific fork of SpenceKonde/DxCore).
 * (C) Workshop Asahi 2026.  DxCore is (C) Spence Konde, LGPL 2.1 (see LICENSE.md).
 *
 * Why this file exists
 * --------------------
 * Kunai routes I2C (TWI0) to PA0/PA1 = the XIAO pads D4/D5. On the AVR DU the
 * chip's DEFAULT TWI mux (PORTMUX_TWI0_DEFAULT_gc) is PA2/PA3; PA0/PA1 are the
 * ALT3 position. DxCore's Wire library has a compile-time *default swap* only for
 * the USARTs (HWSERIALn_MUX_DEFAULT) - there is NO equivalent for TWI, and
 * Wire.begin() does not change PORTMUX (TWI0_ClearPins() only reads it). So a
 * plain Wire.begin() would otherwise talk on the chip's reset-default pins
 * (PA2/PA3), not the board's I2C pads.
 *
 * initVariant() therefore selects TWI0 ALT3 (PA0/PA1) once at boot. Wire.begin()
 * then reads that PORTMUX value and operates on PA0/PA1 with no swap() call. The
 * choice persists until the sketch explicitly calls Wire.swap()/Wire.pins().
 *
 * This is the same force-link pattern Tachi/Tsurugi use: Arduino archives the
 * variant-folder objects into core.a, and the core's main.cpp supplies a *weak*
 * initVariant(). An archived strong symbol does NOT override a weak one already
 * provided by a linked object, so without help this whole TU is silently dropped
 * at link time. boards.txt passes  -Wl,-u,wazamono_kunai_variant_keep , which
 * forces the linker to pull this member; once pulled, the strong initVariant()
 * here overrides the weak default. (Confirmed necessary, including under -flto.)
 *
 * The on-board LED (PD4) is left alone: at reset all pins are high-impedance
 * inputs, so the LED is already off, and not driving it here keeps this file
 * independent of the LED's electrical polarity.
 */

#include <Arduino.h>

#if defined(WAZAMONO_KUNAI_PINOUT)

/* ---- Force-link marker (see header comment) ---- */
extern "C" { __attribute__((used)) char wazamono_kunai_variant_keep = 0; }

/* initVariant(): runs once after init(), before setup() and before the USB stack
 * starts. Select TWI0 ALT3 (PA0/PA1 = D4/D5) as the power-on I2C position. */
void initVariant(void) {
    PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & ~PORTMUX_TWI0_gm) | PORTMUX_TWI0_ALT3_gc;
}

#endif /* WAZAMONO_KUNAI_PINOUT */
