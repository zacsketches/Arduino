/* 
 * 3/23 UNSTABLE
 * COMPLETE - A sketch to see the data from the sparkfun breakout board for the HMC588L.
 * Add data logging into an array when a switch is turned on.
 * by: Zac Staples on 3/23/13
 * 
 * based on the work of Jordan McConell from SparkFun Electronics
 * 
 * Analog 4 I2C SDA
 * Analog 5 I2C SCL
 */
 
#include <Wire.h>             //I2C Arduino Library
#define address 0x1E          //0011110b, I2C 7bit address of HMC5883

int const LEDPin = 13;
int const onSwitch = 5;
int val = 0;
boolean LOGGING_ON = false;

boolean const INVERT_X = false;    //due to mounting might be necessary to invert x

void setup() {
	Serial.begin(9600);                  //initialize serial communication
        Wire.begin();                        //initialize I2C bus
        //Put the HMC5883 IC into the correct operating mode
         Wire.beginTransmission(address); //open communication with HMC5883
         Wire.write(0x02); //select mode register
         Wire.write(0x00); //continuous measurement mode
         Wire.endTransmission();
         
         pinMode(LEDPin, OUTPUT); 
         pinMode(onSwitch, INPUT);
}

void loop() {
   int Z = getHeading();
   showHeading(Z);
 
 delay(50);
}

int getHeading() {
  int x, y, z, Z;  //triple axis data.  Z is heading

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //x msb
    x |= Wire.read();   //x lsb
    z = Wire.read()<<8; //z msb
    z |= Wire.read();   //z lsb
    y = Wire.read()<<8; //y msb
    y |= Wire.read();   //y lsb
  }
    
  //convert to heading
  if (x>0 && y==0) {   //000 degrees
    Z = 0; 
  }  
  if (x>0 && y>0) {   //001-089 degrees
    Z = (atan2(y, x)) * 180 / M_PI;
  }
  if (x==0 && y>0) {   //090 degrees
    Z = 90;
  }
  if (x<0 && y>0) {    //91-179 degrees
    Z = (atan2(y, x)) * 180 / M_PI;
  }
  if (x<0 && y==0) {   //180 degrees
    Z = 180; 
  }
  if (x<0 && y<0) {    //181-269 degrees
    Z = 360 + (atan2(y, x)) * 180 / M_PI;
  }
  if (x==0 && y<0) {   //270 degrees
    Z = 270;
  }
  if (x>0 && y<0) {    //271-359 degrees
   Z = 360 + (atan2(y, x)) * 180 / M_PI; 
  }

  //Invert X if required by the sensor alignment in the project  
  if (INVERT_X) {
    Z = Z-180;
    if (Z < 0 ) {
      Z = 360 + Z; 
    }
  }
//  showXYZWithLabels(x, y, z);
  showXYZWithoutLabels(x, y, z);
  return Z; 
}

void showHeading(int Z) {
   Serial.print("\t H is: ");
   Serial.println(Z); 
}

void showXYZWithLabels(int x, int y, int z) {
   Serial.print("x: ");
   Serial.print(x);
   Serial.print("\ty: ");
   Serial.print(y);
   Serial.print("\t z: ");
   Serial.print(z); 
}

void showXYZWithoutLabels(int x, int y, int z) {
   Serial.print(x);
   Serial.print("\t");
   Serial.print(y);
   Serial.print("\t");
   Serial.print(z); 
}
