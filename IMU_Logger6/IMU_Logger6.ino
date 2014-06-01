
#include <Wire.h>                 //I2C Arduino Library

void setup() {
   Serial.begin(9600);                  //initialize serial communication
   Wire.begin();                        //initialize I2C bus        
}

void loop() {
  int H = IMU.getHeading();
  delay(250);
  Serial.print("Heading is: ");
  Serial.println(H);
}

