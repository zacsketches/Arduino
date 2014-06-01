#include <Servo.h>
#include "Scanner.h"
 
Servo test;

Scanner scanner;

void setup() {
  Serial.begin(9600);
  delay(300);
//  test.attach(9);
  scanner.attach(9, 8);
 
}

void loop() {
  delay(2000);
//  test.write(150);
  scanner.write(150);
  delay(2000);
  scanner.write(30);
//  test.write(30);
}
