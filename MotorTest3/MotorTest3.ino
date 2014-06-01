/* The test of my Rover5 Motor Controller Library */
#include <Rover5Motor2.h>

Rover5Motor2 m1 = Rover5Motor2(5, 4);
Rover5Motor2 m2 = Rover5Motor2(6, 7);
StateVector FWD_FULL = {0, 250};
StateVector FWD_HALF = {0, 125};
StateVector STOP = {0, 0};

void setup() {
  delay(2000);
}
void loop() {
  delay(3000);
  m1.updateState(FWD_FULL);
  m2.updateState(FWD_FULL);
  delay(3000);
  
  m1.updateState(STOP);
  m2.updateState(STOP);
  delay(3000);
  
  m1.updateState(FWD_FULL);
  m2.updateState(FWD_HALF);
  delay(3000);
  m2.updateState(FWD_FULL);
  m1.updateState(FWD_HALF);
  delay(3000);
  
}
