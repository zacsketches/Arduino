//test to build the routine that divides the 180 degree
//servo rotation into slices and then rotates the servo to each position.
//This code prevents driving the servo all the way to 180 and 0 degrees.
//180 divided by segments must be an integer

#include <Servo.h>

int span=180; //how wide is the sweep in degrees
Servo jackServo;
int segments=9; //how many segments will the span be divided into
int servoPin=2; //where is the servo plugged in
int servoSafetyDelay=500; //protects the servo from getting commands too quickly

void setup () {
 jackServo.attach(servoPin);
 Serial.begin(9600);
 swingLeft();
}

void loop() {
  int segment = (span/segments);
  int observationPoints = segments+1;
  
  int pan=1; //used to control sweep direction
  Serial.println("I'm out of the for statement.");
  for (int i=1; i>-1; i=i+pan) {
    int point = abs(i)*segment;
    if (point==180) pan=-1;  //this reverses the pan direction
    if (point>175) point=175;  //protects the edges of the servo rotation
    if (point<5) point=5;  //protects the edges of the servo rotation
    //rotateToPoint(point);
    Serial.println(point);
    //Serial.print("i is=");
    //Serial.println(i);
  }
}

void swingLeft() {
  jackServo.write(0);
  delay(servoSafetyDelay);
}

void swingRight() {
  jackServo.write(175); 
}

void rotateToPoint(int point) {
  jackServo.write(point);  
  delay(servoSafetyDelay);
}
