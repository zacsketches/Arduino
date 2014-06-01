#include "Arduino.h"
#include "SixDOF_IMU.h"
#include "Wire.h"

SixDOF_IMU::SixDOF_IMU() {
	// user must call the configureIMU() method in the setup routine of the sketch
}

//public methods
int SixDOF_IMU::getHeading() {
    int H, Hcomp, Huncomp;
    updateHMC();
    updateADXL();
    updatePitchAndRoll(accelX, accelY, accelZ);
    
    if(USE_TILT_COMPENSATION) {
      H = getTiltCompensatedHeading(headingX, headingY, headingZ, pitch, roll); 
    } else {
      H = getTwoDOFHeading(headingX, headingY);
    }
    return H;  
}

int SixDOF_IMU::getTiltCompensationSetting(){
	int val = USE_TILT_COMPENSATION;
	return val;
}

void SixDOF_IMU::configureIMU() {
    writeTo(HMC5883, 0x02, 0x00);         //set continuous measurement mode      
    writeTo(ADXL345, 0x2D, 0);            //Turning on the ADXL345.  See datasheet page 16    
    writeTo(ADXL345, 0x2D, 16);  
    writeTo(ADXL345, 0x2D, 8);
    writeTo(ADXL345, 0x31, 0);            //set G range.  Datasheet page 17
	// Serial.println("IMU Configured.");
	// Serial.println("-----------------------------------");

}

//private methods
int SixDOF_IMU::getTiltCompensatedHeading(int x, int y, int z, float theta, float phi) {
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
  
int SixDOF_IMU::getTwoDOFHeading(int x, int y) {
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
  
void SixDOF_IMU::updatePitchAndRoll(int x, int y, int z) {
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

void SixDOF_IMU::updateADXL() {
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
  
void SixDOF_IMU::updateHMC() {
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
  
void SixDOF_IMU::writeTo(int device, byte address, byte val) {
    Wire.beginTransmission(device); // start transmission to device 
    Wire.write(address);            // send register address
    Wire.write(val);                // send value to write
    Wire.endTransmission();         // end transmission
}
 

