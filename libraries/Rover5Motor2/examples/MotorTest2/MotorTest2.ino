/* The test of my Rover5 Motor Controller Library */
#include <Rover5Motor2.h>

Rover5Motor2 m1 = Rover5Motor2(5, 4);
Rover5Motor2 m2 = Rover5Motor2(6, 7);
StateVector FWD_FULL = {0, 250};
StateVector FWD_HALF = {0, 125};

void setup() {
  m1.updateState(FWD_FULL);
  m2.updateState(FWD_FULL);
}
void loop() {
  m1.drive();
//  m2.drive();
  delay(3000);
  m1.updateState(FWD_HALF);
  m1.drive();
  m2.stop();
  delay(3000);
  m1.stop();
}
