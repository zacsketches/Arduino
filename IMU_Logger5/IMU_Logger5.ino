/* 
 * 4/10 STABLE
 *  - Tilt compensated heading based on matrix solution for 6 DOF with solution accuracy across the full
 *  pitch and roll spectrum.
 *  - Sketch is designed to take readings from an HMC5883L magnetometer and an ADXL345
 * accelerometer controlled via an I2C master Arduino Uno and save them to a micro SD card.
 *  - chips are accessed via breakout boards from SparkFun electronics.
 *  - Slide switch plugged into Uno D8 controls whether data is logged to SD or printed to serial port.  
 *  - If you plan to use SD logging then turn on the boolean LOG_TO_SD and use the switch
 * to control starting and stopping the data collection.
 *  - MicroSD card connected via Arduino ethernet shield controlled via SPI.
 * NOTE: Use of both SPI and I2C takes a lot of pins && the SD library seems to take up a lot of 
 * room.  Might be more compact with a diff't style of SD logger like Sparkfun OpenLog.
 * by: Zac Staples
 * 
 * Some HMC5883L code based on the work of Jordan McConell from SparkFun Electronics 
 * licensed under: OSHW 1.0 http://freedomdefined.org/OSHW
 * 
 * Bit shifting for ADXL data based on work at
 * http://codeyoung.blogspot.com/2009/11/adxl345-accelerometer-breakout-board.html
 * 
 * HMC Breakout      UNO Pin
 *   GND ------------> GND
 *   VCC ------------> 3.3V
 *   SDA ------------> I2C SDA Bus
 *   SCL ------------> I2C SCL Bus
 
 * ADXL Breakout     UNO Pin
 *     GND ----------> GND
 *     VCC ----------> 3.3V
 *     CS  ----------> 3.3V    Sets devis to I2C mode (datasheet pg10)
 *     INT1 ---------> Not Connected
 *     INT2 ---------> Not Connected
 *     SDO ----------> GND     Sets device address to 0x53 (datasheet pg10)
 *     SDA ----------> I2C SDA Bus
 *     SCL ----------> I2C SCL Bus
 *
 * SD shield attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * microSD CS - pin 4
 * Ethernet CS = pin 10
 *
 * NOTE: SDA and SCL bus connections both require a 10K pullup resistor for best performance
 * with multiple devices on the bus.
 */
#include <SD.h>                   //SPI communications with an SD card
#include <Wire.h>                 //I2C Arduino Library

//NAME OF THE SD LOG FILE TO CREATE
char logFile[ ] = "IMU004.txt";

//CONFIGURATION OPTIONS
boolean LOG_TO_SD = false;                        
boolean LOG_TO_SERIAL = true;
boolean USE_EXTENDED_LOGGING = true;             //extended logging includes labels only on Serial
boolean USE_TILT_COMPENSATION = true;

//AXIS ALIGNMENT & ADDRESSES...DO NOT CHANGE
boolean const INVERT_HEADING_X = false;          //my rotation calcs are based on the industry standard
boolean const INVERT_HEADING_Y = true;           //for handheld devices with a coord axis of 
boolean const INVERT_HEADING_Z = true;           //North (x), East (y) and Down (z) as positive
boolean const INVERT_ACCEL_X = true;             //for both the magnetometer and the accelerometer
boolean const INVERT_ACCEL_Y = false;
boolean const INVERT_ACCEL_Z = false;
const int HMC5883 = 0x1E;         //I2C 7bit address of HMC5883
const int ADXL345 = 0x53;         //I2C address for ADXL345 with SDO tied to ground
const int HMC5883_X_REG = 0x03;   //Datasheet pg 11
const int ADXL345_X_REG = 0x32;   //Datasheet pg 14
const int DATA_BUFFER_BYTES = 6;  //Buffer for ADXL data
const int LOGGER_SWITCH = 8;      //Digital I/O port for the log on/off switch

//Compensation data of 7 Apr 2013
float const Vx = -97.2;
float const Vy = 1.0;
float const Vz = -34.4;
float const B = 485.8;

//Global Variables
File myFile;
int headingX, headingY, headingZ;
int accelX, accelY, accelZ;
float pitch;
float roll;

void setup() {
   Serial.begin(9600);                  //initialize serial communication
   Wire.begin();                        //initialize I2C bus

   if (configureIMULogger()) {
      Serial.println();
      Serial.println("SD Successfully Initialized!"); 
   } else {
      Serial.println();
      Serial.println("SD Initialization failed!"); 
      LOG_TO_SD = false;                //turn off SD logging since it didn't initialize
   }		     
        
}

void loop() {
  int H = getHeading(USE_TILT_COMPENSATION);
  delay(250);
}

int getHeading(boolean compensateHeading) {
  int H, Hcomp, Huncomp;
  updateHMC();
  updateADXL();
  updatePitchAndRoll(accelX, accelY, accelZ);
  
  Huncomp = getTwoDOFHeading(headingX, headingY);
  Hcomp = getTiltCompensatedHeading(headingX, headingY, headingZ, pitch, roll);
  
  if(compensateHeading) {
    H = Hcomp; 
  } else {
    H = Huncomp;
  }
 
  if(LOG_TO_SD) {
    int val = digitalRead(LOGGER_SWITCH);
    if (val == HIGH) {    
      logToSD(headingX, headingY, headingZ, Huncomp, Hcomp, accelX, accelY, accelZ, pitch, roll);
    }
  } 
  if (LOG_TO_SERIAL) {
    if(USE_EXTENDED_LOGGING) {
       showHMCXYZWithLabels(headingX, headingY, headingZ);
       showHeadingWithLabels(Huncomp);
       showCompHeadingWithLabels(Hcomp);
       showADXLXYZWithLabels(accelX, accelY, accelZ);
       showPitchAndRollWithLabels(pitch, roll);
    } else {
       showHMCXYZ(headingX, headingY, headingZ);
       showHeading(Huncomp);
       showCompHeading(Hcomp);
       showADXLXYZ(accelX, accelY, accelZ);
       showPitchAndRoll(pitch, roll);
    }   
    Serial.println(); 
  }
  return H;  
}

boolean configureIMULogger() {
  boolean IMUconfigured = true;
  // On the Arduino Ethernet Shield, SD CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(10, OUTPUT);
  
  pinMode(LOGGER_SWITCH, INPUT);

  writeTo(HMC5883, 0x02, 0x00);         //set continuous measurement mode      
  writeTo(ADXL345, 0x2D, 0);            //Turning on the ADXL345.  See datasheet page 16    
  writeTo(ADXL345, 0x2D, 16);  
  writeTo(ADXL345, 0x2D, 8);
  writeTo(ADXL345, 0x31, 0);            //set G range.  Datasheet page 17
  
  if (LOG_TO_SD) {   
    if (!SD.begin(4)) {
      IMUconfigured = false;
    } else {
      IMUconfigured = true;     
    }     
  }
  return IMUconfigured;         
}

void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device); // start transmission to device 
  Wire.write(address);            // send register address
  Wire.write(val);                // send value to write
  Wire.endTransmission();         // end transmission
}

int getTwoDOFHeading(int x, int y) {
  //convert measured magnetometer data to heading
  int H;
  if (y>0) {
    H = atan2(y, x)*180/PI;
    H = 360 - H; 
  }
  if (y<=0) {
    H = atan2(y, x)*180/PI; 
    H = H*-1;
  }
  //The ATAN2 function will return headings of 360 and -180 so we correct for these
  if(H == 360) {
     H = 0; 
  }
  if(H == -180) {
    H = 180; 
  }
  return H;
}

int getTiltCompensatedHeading(int x, int y, int z, float theta, float phi) {
  /* Credit Freescale Semiconductors Application Note 4248 by Talat Ozyagcilar for the
   * math to build this implementation.  Psi is the yaw angle or heading.  This algorithm
   * requires Vz, Vy and Vz which are the vector components of V, the hard iron effects
   * of ferrous objects on/near the installation of the sensor.  V can be computed via 
   * spreadsheet by solving the matrix (Bp-V)^T * (Bp-V) = B^2, where Bp is the compass
   * readings and V is the hard iron offset.  Freescale app note AN4246 is helpful here
   * as well.
   *  - Identification of the duplicate region from psi = PI through psi = PI/2 is my
   * solution arrived at independently.
   *
   *                                  (Bz - Vz)sin(phi)-(By-Vy)cos(phi)
   * tan(psi) = ----------------------------------------------------------------------------
   *             (Bx - Vx)cos(theta) + (By-Vy)sin(theta)sin(phi) + (Bz-Vz)sin(theta)cos(phi)
   *
   */
  int H;
  boolean duplicateRegion = false;
  float psi;
  theta = theta*PI/180;          //convert to radians
  phi = phi *PI/180;     
  float psiNum = (z-Vz)*sin(phi)-(y-Vy)*cos(phi);
  float psiDenom = (x-Vx)*cos(theta) + (y-Vy)*sin(theta)*sin(phi) + (z-Vz)*sin(theta)*cos(phi);
  psi = atan2(psiDenom, psiNum);
  //duplicate result for this region we need to manage
  if (psi < PI && psi > PI/2) {
    duplicateRegion = true;
    psi = 5*PI/2 - psi;
  }
  if (duplicateRegion == false) {
    psi = PI/2 - psi;  
  }
  psi = psi*180/PI;  
  H = int(psi);
  return H;
}

void updatePitchAndRoll(int x, int y, int z) {
    /* Credit Freescale Semiconductors, Application Note AN3461 by Mark Pedley for the 
     * math to build this implementation.  This note is the best I've seen on the web. 
     * From the aerospace rotation sequence Rxyz where Theta = pitch angle and Phi = roll angle.
     * mu is a small fraction multiplied with the Gx value to ensure that the denominator of 
     * the Phi solution never comes up zero.  The sign of Gz is used to preserve the sign that is 
     * lost in the squaring function under the radical.  Pitch is limited from -90 to 90 deg while
     * roll is a wider limit of -180 to 180 deg.  This limit on pitch is a typical convention
     * of the aerospace solution to give a unique solution to every set of inputs.
     *
     *                      -Gx
     * tan(theta) = -----------------------  => ATAN solves for Pitch -PI/2 to PI/2
     *                 (Gy^2 + Gz^2)^-2
     *
     *                       Gy
     * tan(phi) = ---------------------------  => ATAN2 solves for Roll -PI to PI
     *            sign(Gz) (Gz^2 + mu*Gx^2)^-2
     */
     float thetaDenominator = sqrt(pow(y,2) + pow(z,2));
     float theta = (-1 * x)/ thetaDenominator;
     theta = atan(theta); 
     theta = theta *180/PI;
     pitch = theta;    
     float mu = 1/100;
     int signGz = 1;
     if (z<0) {
       signGz = -1;
     }
     float phiDenominator = signGz * sqrt(pow(z,2) + mu*pow(x,2));
     float phi = atan2(y, phiDenominator); 
     phi = phi *180/PI;
     roll = phi;     
}

void updateADXL() {
  int x, y, z;
  byte buff[DATA_BUFFER_BYTES];
  
  //Tell the ADXL345 where to begin reading data
  Wire.beginTransmission(ADXL345);
  Wire.write(ADXL345_X_REG); //select register 3, X MSB register
  Wire.endTransmission();
  
  Wire.requestFrom(ADXL345, 6);       // request 6 bytes from device
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  
  //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
  
  if(INVERT_ACCEL_Z) {
    z = z*-1;
  }
  if(INVERT_ACCEL_Y) {
    y = y*-1; 
  }
  if(INVERT_ACCEL_X) {
    x = x*-1; 
  }
  accelX = x;
  accelY = y;
  accelZ = z;
}

void updateHMC() {
  int x, y, z;

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883);
  Wire.write(HMC5883_X_REG);
  Wire.endTransmission();
  
 //Read data from each axis, 2 registers per axis.  Most significant Byte first!!!
  Wire.requestFrom(HMC5883, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //x msb
    x |= Wire.read();   //x lsb
    z = Wire.read()<<8; //z msb
    z |= Wire.read();   //z lsb
    y = Wire.read()<<8; //y msb
    y |= Wire.read();   //y lsb
  }
  if(INVERT_HEADING_Z) {
    z = z*-1;
  }
  if(INVERT_HEADING_Y) {
    y = y*-1; 
  }
  if(INVERT_HEADING_X) {
    x = x*-1; 
  }
  headingX = x;
  headingY = y;
  headingZ = z;
}

void logToSD(int Hx, int Hy, int Hz, int H, int cH, int Ax, int Ay, int Az, float theta, float phi) {
  myFile = SD.open(logFile, FILE_WRITE);
  if (myFile) {
     Serial.println("Writing data to SD");
     myFile.print(Hx);
     myFile.print("\t");
     myFile.print(Hy);
     myFile.print("\t");
     myFile.print(Hz);
     myFile.print("\t");
     myFile.print(H);
     myFile.print("\t");
     myFile.print(cH);
     myFile.print("\t");
     myFile.print(Ax);
     myFile.print("\t");
     myFile.print(Ay);
     myFile.print("\t");
     myFile.print(Az);
     myFile.print("\t");     
     myFile.print(theta);
     myFile.print("\t");
     myFile.print(phi);
     myFile.print("\n");
     // close the file...must do this to actually write the data
     myFile.close();
  } else {
    if (LOG_TO_SERIAL) {
      Serial.println("error opening log file");
    }
  }
}

void showHMCXYZWithLabels(int x, int y, int z) {
   char str[512];
   sprintf(str, "HMC x: %d \tHMC y: %d \tHMC z: %d", x, y, z);
   Serial.print(str);
}
void showHeadingWithLabels(int Z) {
   char str[512];
   sprintf(str, "\tHead: %d ", Z);
   Serial.print(str);
}
void showCompHeadingWithLabels(int compZ) {
   char str[512];
   sprintf(str, "\tComp: %d ", compZ);
   Serial.print(str);  
}
void showADXLXYZWithLabels(int x, int y, int z) {
   char str[512];
   sprintf(str, "\tADXL x: %d \tADXL y: %d \tADXL z: %d", x, y, z);
   Serial.print(str); 
}
void showPitchAndRollWithLabels(float theta, float phi) {
   Serial.print("\tPitch is: "); 
   Serial.print(theta);
   Serial.print("\tRoll is: ");
   Serial.print(phi);
}
void showHMCXYZ(int x, int y, int z) {
   char str[512];
   sprintf(str, "%d\t%d\t%d", x, y, z);
   Serial.print(str);
}
void showHeading(int Z) {
   char str[512];
   sprintf(str, "\t%d", Z);
   Serial.print(str);
}
void showCompHeading(int compZ) {
   char str[512];
   sprintf(str, "\t%d", compZ);
   Serial.print(str);  
}
void showADXLXYZ(int x, int y, int z) {
   char str[512];
   sprintf(str, "\t%d\t%d\t%d", x, y, z);
   Serial.print(str); 
}
void showPitchAndRoll(float theta, float phi) {
   Serial.print("\t"); 
   Serial.print(theta);
   Serial.print("\t");
   Serial.print(phi);
}

