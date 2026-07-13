/* CustomLogic / ThreeInputOR
 *
 * A 3-input OR gate: OUT is HIGH while at least one input is HIGH.
 * Change OR to AND, XOR, NAND, NOR or XNOR to try the other gates -
 * nothing else in the sketch needs to change.
 *
 * Pins of the CustomLogic unit:
 *            IN0        IN1        IN2        OUT
 *   Tachi    A3         A2         A1         A0
 *   Tsurugi  D5         D6         D9         D10
 *   Kunai    D4         D5         D3         D2
 */
#include <CustomLogic.h>

void setup() {
  Serial.begin(115200);
  CustomLogic.begin(OR, 3);   // OUT = IN0 OR IN1 OR IN2
}

void loop() {
  // read() reports the current state of the OUT pin.
  Serial.println(CustomLogic.read() ? "OUT = HIGH" : "OUT = LOW");
  delay(500);
}
