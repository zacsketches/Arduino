/*
An Arduino code example for interfacing with the HMC5883

by: Jordan McConnell
 SparkFun Electronics
 created on: 6/30/11
 license: OSHW 1.0, http://freedomdefined.org/OSHW

Analog input 4 I2C SDA
Analog input 5 I2C SCL
*/

#include <Wire.h> //I2C Arduino Library
#include <math.h> //included to get access to arctangent

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
  

}

void loop(){
  
  int x,y,z; //triple axis data

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  
 
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
  /*
  //Print out values of each axis
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
  */
  
  //convert to heading
  int Z;
  if (x==0 && y<0) {
    Z = 270;
  }
  if (x==0 && y>0) {
    Z = 90;
  }
  if (x<0 && y<0) {
    Z = 360 + (atan2(y, x)) * 180 / M_PI;
  }
  if (x<0 && y>0) {
    Z = (atan2(y, x)) * 180 / M_PI;
  }
  if (x>0 && y<0) {
   Z = 360 + (atan2(y, x)) * 180 / M_PI; 
  }
  if (x>0 && y==0) {
   Z = 0; 
  }
  if (x>0 && y>0) {
    Z = (atan2(y, x)) * 180 / M_PI;
  }
  Serial.print("The heading is: ");
  Serial.println(Z);
  delay(250);
}
