/*
 Library to allow interaction with my six degree of freedom IMU.
 IMU is built on HMC5883 compass and ADXL345 accelerometer.

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
* NOTE: SDA and SCL bus connections both require a 10K pullup resistor for best performance
* with multiple devices on the bus.
*/
#ifndef SixDOF_IMU_h
#define SixDOF_IMU_h

#include "Arduino.h"

//CONFIGURATION OPTIONS
#define USE_TILT_COMPENSATION 1

//AXIS ALIGNMENT & ADDRESSES...DO NOT CHANGE
#define INVERT_HEADING_X 0           //my rotation calcs are based on the industry standard
#define INVERT_HEADING_Y 1           //for handheld devices with a coord axis of 
#define INVERT_HEADING_Z 1           //North (x), East (y) and Down (z) as positive
#define INVERT_ACCEL_X 1             //for both the magnetometer and the accelerometer
#define INVERT_ACCEL_Y 0
#define INVERT_ACCEL_Z 0
#define HMC5883 0x1E         //I2C 7bit address of HMC5883
#define ADXL345 0x53         //I2C address for ADXL345 with SDO tied to ground
#define HMC5883_X_REG 0x03   //Datasheet pg 11
#define ADXL345_X_REG 0x32   //Datasheet pg 14
#define DATA_BUFFER_BYTES 6  //Buffer for ADXL data

//Compensation data of 7 Apr 2013
#define Vx -97.2
#define Vy 1.0
#define Vz 34.4
#define B 485.8

class SixDOF_IMU {
	public:
		//constructor
		SixDOF_IMU();
		//public methods
		int getHeading();
		void configureIMU();
		int getTiltCompensationSetting();
	private:
	    //Private Global Variables
	    int headingX;
    	int headingY;
	    int headingZ;
	    int accelX;
		int accelY;
	    int accelZ;
	    float pitch;
	    float roll;
  
  		//private methods
		int getTiltCompensatedHeading(int x, int y, int z, float theta, float phi);
		int getTwoDOFHeading(int x, int y);
		void updatePitchAndRoll(int x, int y, int z);
		void updateADXL();
		void updateHMC();
		void writeTo(int device, byte address, byte val);
};

#endif