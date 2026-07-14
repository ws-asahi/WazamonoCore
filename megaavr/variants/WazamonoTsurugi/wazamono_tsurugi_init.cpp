/* wazamono_tsurugi_init.cpp - board-specific hardware init for Wazamono Tsurugi
 * ---------------------------------------------------------------------------
 * Part of WazamonoCore (a product-specific fork of SpenceKonde/DxCore).
 * (C) Workshop Asahi 2026.  DxCore is (C) Spence Konde, LGPL 2.1 (see LICENSE.md).
 *
 * Purpose
 *   Board-specific initialization hook, run once between init() and setup().
 *   CURRENTLY EMPTY - kept on purpose: the force-link plumbing (marker symbol
 *   here + -Wl,-u in boards.txt) stays wired up so that future board-specific
 *   init can simply be written into initVariant() below and it will run,
 *   without anyone having to rediscover the archive/weak-symbol pitfall:
 *   Arduino archives variant objects into core.a, and an archived strong
 *   initVariant() does NOT override the core's weak one unless something
 *   forces this member to be pulled in.
 *
 * History
 *   Until board rev. B this file configured a CCL/EVSYS hardware mirror that
 *   reproduced D13 (PD6) on PC3 to drive the on-board LED. Measurement then
 *   showed PC3's output driver is powered from VDD (not VUSB), PC3 became the
 *   plain GPIO D7, and the LED moved to an op-amp buffer on D13 - the same
 *   arrangement as the Arduino Uno R3 - so the mirror was removed whole.
 *   EVSYS channel 0 and CCL LUT1 are free for sketches.
 */

#include <Arduino.h>

#if defined(WAZAMONO_TSURUGI_PINOUT)

/* Force-link marker: boards.txt passes -Wl,-u,wazamono_tsurugi_variant_keep,
 * which makes the linker pull this archive member (and initVariant() with it). */
extern "C" { __attribute__((used)) char wazamono_tsurugi_variant_keep = 0; }

/* initVariant() is a weak no-op in the core (cores/dxcore/main.cpp); this strong
 * definition overrides it and runs once, immediately after init(), before setup(). */
void initVariant(void) {
  /* No board-specific hardware init at present. */
}

#endif /* WAZAMONO_TSURUGI_PINOUT */
