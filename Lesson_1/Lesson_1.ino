//Written by Greyson, Hayden and Dad on Feb 13th to learn about servos
//We won't use this in any other application

#include <Servo.h>

Servo jackServo;
int servoPin;

void setup()
  {
    servoPin=2;
    
    jackServo.attach(servoPin);
  }
  
void loop()
  {
   jackServo.write(05); 
   delay(1000); 
   jackServo.write(90);
   delay(1000); 
   jackServo.write(175);
   delay(1000);
   jackServo.write(90);
   delay(1000);
  }


