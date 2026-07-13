/* CustomLogic.h - Beginner-friendly CCL logic gates for WazamonoCore.
 *
 * Part of WazamonoCore for the AVR DU-series Wazamono boards.
 * (C) 2026 Yusuke Shimizu - Workshop Asahi. LGPL 2.1.
 *
 * Provenance: this library is an original, independent implementation
 * written from the AVR64DU28/32 data sheet (DS40002548A) and the official
 * Microchip device headers only. It does not use, include, or derive code
 * from the DxCore/megaTinyCore "Logic" library or any other CCL library.
 *
 * Pre-instantiated objects turn a CCL look-up table into a hardware logic
 * gate that keeps working with zero CPU involvement:
 *
 *   CustomLogic.begin(AND);          // OUT = IN0 AND IN1
 *   CustomLogic.begin(OR, 3);        // OUT = IN0 OR IN1 OR IN2
 *   CustomLogic.beginTruthTable(0x96, 3);   // any function of 3 inputs
 *
 * Units and pins (fixed by the CCL hardware):
 *                 IN0        IN1        IN2        OUT
 *   CustomLogic
 *     Tachi       A3 (PD0)   A2 (PD1)   A1 (PD2)   A0 (PD3)
 *     Tsurugi     D5 (PD0)   D6 (PD1)   D9 (PD2)   D10 (PD3)
 *     Kunai       D4 (PA0)   D5 (PA1)   D3 (PA2)   D2 (PA3)
 *   CustomLogic1  (not available on Kunai)
 *     Tachi       D5 (PF0)   D6 (PF1)   D7 (PF2)   D8 (PF3)
 *     Tsurugi     A0 (PF0)   A1 (PF1)   A2 (PF2)   A3 (PF3)
 *
 * Input pins are configured with pull-ups, so you can wire buttons
 * straight to GND; driven logic signals simply override the pull-up.
 */
#ifndef CUSTOMLOGIC_H
#define CUSTOMLOGIC_H

#include <Arduino.h>

#if !defined(WAZAMONO_BOARD_TACHI) && !defined(WAZAMONO_BOARD_TSURUGI) && !defined(WAZAMONO_BOARD_KUNAI)
  #error "CustomLogic supports Wazamono boards only."
#endif

/* Gate types for begin() */
enum CustomLogicGate : uint8_t {
  AND = 0,  /* OUT is HIGH while every input is HIGH            */
  OR,       /* OUT is HIGH while at least one input is HIGH     */
  XOR,      /* OUT is HIGH while an odd number of inputs are HIGH */
  NAND,     /* inverted AND                                      */
  NOR,      /* inverted OR                                       */
  XNOR,     /* inverted XOR                                      */
  NOT       /* OUT is the inverse of IN0 (single input)          */
};

class CustomLogicClass {
public:
  CustomLogicClass(uint8_t lut, const uint8_t *pins); /* internal */

  /* Turn this unit into a logic gate using the first numInputs input
   * pins (1 to 3; NOT always uses just IN0). Unused inputs are ignored
   * regardless of their pin state. The result appears on the OUT pin
   * immediately and permanently - no code runs in loop(). */
  bool begin(uint8_t gate, uint8_t numInputs = 2);

  /* Define the output for every input combination yourself. Bit i of
   * truthTable is the output when the inputs spell the number i
   * (IN2 = bit 2, IN1 = bit 1, IN0 = bit 0). Example: 0b10010110 (0x96)
   * makes a 3-input XOR (odd parity). */
  bool beginTruthTable(uint8_t truthTable, uint8_t numInputs = 3);

  /* Stop the unit and release its pins. */
  void end();

  /* Current state of the OUT pin. */
  bool read();

  /* Call a function when the gate output changes, like attachInterrupt():
   * mode = RISING, FALLING or CHANGE. */
  void attachInterrupt(void (*callback)(), uint8_t mode);
  void detachInterrupt();

private:
  bool configure(uint8_t truth, uint8_t numInputs);
  const uint8_t *_pins;   /* IN0, IN1, IN2, OUT */
  uint8_t _lut;
  uint8_t _numInputs = 0;
  bool _running = false;
};

extern CustomLogicClass CustomLogic;
#if !defined(WAZAMONO_BOARD_KUNAI)
extern CustomLogicClass CustomLogic1;
#endif

#endif
