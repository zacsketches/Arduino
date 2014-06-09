/* Zac Staples May 2013. 
 * First attempt to implement my new IMU class
 */
#include <Wire.h>
#include <SixDOF_IMU.h>

SixDOF_IMU IMU;

void setup() {
  Serial.begin(9600);
  Wire.begin();  //initialize I2C bus
  IMU.configureIMU();
  if (IMU.getTiltCompensationSetting()==1) {
    Serial.println("Using 6DOF tilt compensated heading");
  } else {
    Serial.println("Using 2DOF heading");
  }
   
}

void loop() {
  int H = IMU.getHeading();
  Serial.print("Heading is: ");
  Serial.println(H);
  delay(250);
}  




