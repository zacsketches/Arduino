//test to build the routine that divides the 180 degree
//servo rotation into slices and then rotates the servo to each position.
#include <Servo.h>

int span=180; //how wide is the sweep in degrees
Servo jackServo;
int segments=9; //how many segments will the span be divided into
int servoPin=2; //where is the servo plugged in

void setup () {
 jackServo.attach(servoPin);
 Serial.begin(9600);

}

void loop() {
  swingLeft();
  delay(1000);
  swingRight();
  delay(1000);
  
}

void swingLeft() {
  jackServo.write(5);
}

void swingRight() {
  jackServo.write(175); 
}
