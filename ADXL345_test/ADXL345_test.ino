/* Written by Zac Staples, March 29th, 2013.  
 * Connects the Sparkfun ADXL345 breakout to Arduino Uno and ouputs
 * X, Y, and Z measurements and/or Pitch & Roll data depending
 * on boolean config variables.
 * structure of the code based on work at
 * http://codeyoung.blogspot.com/2009/11/adxl345-accelerometer-breakout-board.html
 * pitch and roll calculations based on a great App Note from Frescale
 * Semiconductors.
 *
 * ADXL Breakout     UNO Pin
 *     GND ----------> GND
 *     VCC ----------> 3.3V
 *     CS  ----------> 3.3V
 *     INT1 ---------> Not Connected
 *     INT2 ---------> Not Connected
 *     SDO ----------> GND     Sets device address to 0x53 (datasheet pg10)
 *     SDA ----------> I2C SDA Bus
 *     SCL ----------> I2C SCL Bus
 */
#include <Wire.h>

#define DEVICE 0x53    //ADXL345 device address
#define ACCEL_DATA_BYTES 6      //num of bytes we are going to read each time (two bytes for each axis)

byte buff[ACCEL_DATA_BYTES] ;     //6 bytes buffer for saving data read from the device
char str[512];           //string buffer to transform data before sending it to the serial port
boolean SHOW_X_Y_Z = false;
boolean SHOW_ROLL_AND_PITCH = true;

void setup() {
  Wire.begin();           // join i2c bus
  Serial.begin(9600);     // start serial for output
  
  writeTo(DEVICE, 0x2D, 0);  //Turning on the ADXL345.  See datasheet page 16    
  writeTo(DEVICE, 0x2D, 16);  
  writeTo(DEVICE, 0x2D, 8);
  writeTo(DEVICE, 0x31, 0);  //set G range.  Datasheet page 17
}

void loop() {
  int regAddress = 0x32;    //first axis-acceleration-data register on the ADXL345
  int x, y, z;
  
  readFrom(DEVICE, regAddress, ACCEL_DATA_BYTES, buff); //read the acceleration data from the ADXL345
  
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes into one int
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
  
  if(SHOW_X_Y_Z) {
    //we send the x y z values as a string to the serial port
    sprintf(str, "X is: %d \tY is: %d \tZ is: %d", x, y, z);  
    Serial.print(str);
    Serial.println();
  }
  
  if(SHOW_ROLL_AND_PITCH) {
    /* Credit Freescale Semiconductors, Application Note AN3461 by Mark Pedley for the 
     * math to build this implementation.  This note is the best I've seen on the web. 
     * From the aerospace rotation sequence Rxyz where Theta = pitch angle and Phi = roll angle.
     * mu is a small fraction multiplied with the Gx value to ensure that the denominator of 
     * the Phi solution never comes up zero.  The sign of Gz is used to preserve the sign that is 
     * lost in the squaring function under the radical.  Pitch is limited from -90 to 90 deg while
     * roll is a wider limit of -180 to 180 deg.  This limit on pitch is a typical convention
     * of the aerospace solution to give a unique solution to every set of inputs.
     *
     *                       Gy
     * tan(phi) = ---------------------------  = Roll
     *            sign(Gz) (Gz^2 + mu*Gx^2)^-2
     *
     *                      -Gx
     * tan(theta) = -----------------------  = Pitch
     *                 (Gy^2 + Gz^2)^-2
     *
     */
     float mu = 1/100000;
     int signGz = 1;
     if (z<0) {
       signGz = -1;
     }
     float phiDenominator = signGz * sqrt(pow(z,2) + mu*pow(x,2));
     float phi = atan2(y, phiDenominator); 
     phi = phi *180/PI;     
     float thetaDenominator = sqrt(pow(y,2) + pow(z,2));
     float theta = atan2(-1 * x, thetaDenominator); 
     theta = theta *180/PI;
     theta = theta * -1;        //this is my personal pref to have uphill be positive pitch 
     Serial.print("Pitch is: ");
     Serial.print(theta);
     Serial.print("\t\tRoll is: ");
     Serial.println(phi);
  }
  //It appears that delay is needed in order not to clog the port
  delay(15);
}

void writeTo(int device, byte address, byte val) {
   Wire.beginTransmission(device); //start transmission to device 
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
}

void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
  
  Wire.beginTransmission(device); //start transmission to device (initiate again)
  Wire.requestFrom(device, num);    // request 6 bytes from device
  
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}
