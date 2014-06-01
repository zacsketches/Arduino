/* 
 * 3/23 STABLE.
 * If switch plugged into Uno D5 is on the sketch will log compass data to SD card.
 * COMPLETE - A sketch to see the data from the sparkfun breakout board for the HMC588L.
 * COMPLETE - Add data logging into an SD card when the switch is turned on.
 * by: Zac Staples on 3/23/13
 * 
 * based on the work of Jordan McConell from SparkFun Electronics licensed under:
 * OSHW 1.0 http://freedomdefined.org/OSHW
 * 
 * Analog 4 I2C SDA
 * Analog 5 I2C SCL
 * Digital 5 Data Logging on/off switch
 *
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 4
 
 */
#include <SD.h>
#include <Wire.h>             //I2C Arduino Library

#define address 0x1E          //0011110b, I2C 7bit address of HMC5883

//based on compensation data of 24 Mar 2013
int const X_OFFSET = 47;
int const Y_OFFSET = -39;
float const X_SCALE_FACTOR = 1.221;      
float const Y_SCALE_FACTOR = 1.000;      

File myFile;
char logFile[ ] = "test1.txt";

int const HMC5883LoggerSwitch = 5;
int val = 0;
boolean USE_SWITCH = true;
boolean USE_EXTENDED_LOGGING = false;
boolean SHOW_COMPUTED_HEADING = false;
boolean LOG_TO_SD = true;
boolean APPLY_SCALE_AND_OFFSET = false;

boolean const INVERT_X = true;    //due to mounting might be necessary to invert x

void setup() {
	Serial.begin(9600);                  //initialize serial communication
        Wire.begin();                        //initialize I2C bus
        //Put the HMC5883 IC into the correct operating mode
         Wire.beginTransmission(address); //open communication with HMC5883
         Wire.write(0x02); //select mode register
         Wire.write(0x00); //continuous measurement mode
         Wire.endTransmission();
         
         // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
         // Note that even if it's not used as the CS pin, the hardware SS pin 
         // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
         // or the SD library functions will not work. 
         pinMode(10, OUTPUT);
         
         pinMode(HMC5883LoggerSwitch, INPUT);
         if(LOG_TO_SD) {
           if (!SD.begin(4)) {
              Serial.println("SD initialization failed!");
           } else {
              Serial.println("SD successfully initialized.");        
           } 
         }
         
}

void loop() {
  if (USE_SWITCH) {
    val = digitalRead(HMC5883LoggerSwitch);
    if (val == HIGH) {
       int Z = getHeading();
       if(SHOW_COMPUTED_HEADING) {
         showHeading(Z);
       }
    }
  } else {
    int Z = getHeading();
    if(SHOW_COMPUTED_HEADING) {
         showHeading(Z);
    }
  }
  Serial.println();
 
 delay(250);
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
  
  //compensate X and Y
  if(APPLY_SCALE_AND_OFFSET) {
      x = applyOffset(x, 0);
      y = applyOffset(y, 1);  
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
  if(USE_EXTENDED_LOGGING) {
    showXYZWithLabels(x, y, z);
  } else {
    showXYZWithoutLabels(x, y, z);
  }
  
  if(LOG_TO_SD) {
    logToSD(x, y, z); 
  }
  return Z; 
}

void logToSD(int x, int y, int z) {
  myFile = SD.open(logFile, FILE_WRITE);
  if (myFile) {
     Serial.println("Writing HMC data to SD");
     myFile.print(x);
     myFile.print("\t");
     myFile.print(y);
     myFile.print("\t");
     myFile.print(z);
     myFile.print("\n");
     // close the file...must do this to actually write the data
     myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening log file");
  }

}

int applyOffset(int coord, int selector) {
  //coord is the value coming in.  Selector is 0 for the x coord and 1 for the y coord
  float scaleFactor;
  int offset;
  if(selector) {
    scaleFactor = Y_SCALE_FACTOR;
    offset = Y_OFFSET;
  }  else {
    scaleFactor = X_SCALE_FACTOR;
    offset = X_OFFSET;
  }
  coord = scaleFactor * coord + offset;
  return coord;
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
