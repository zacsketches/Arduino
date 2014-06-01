//Servo control example for moving between two positions.

#include <Servo.h>
const int duration = 3000;  //milliseconds
const int closed = 50;      //degrees
const int opened = 160;     //degrees

Servo servo;

void setup() {
  servo.attach(9);
}

void loop() {
  servo.write(opened);
  delay(duration);
  servo.write(closed);
  delay(duration);
}

