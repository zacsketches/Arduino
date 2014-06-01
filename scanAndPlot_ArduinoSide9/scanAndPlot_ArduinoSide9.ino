/* 3/21.  UNSTABLE.
* Designed to work with mapWithComms_ProcessingSide13
* COMPLETE - Get heading off I2C bus from HMC5883 digi compass...this is a cool step.
* COMPLETE - Add servo offset to findX and findY methods to balance the scan index points left and right
* COMPLETE - First challenge in this spiral is to send heading data to Processing when requested.
* COMPLETE - Next challenge for this spiral is using the scan complete data more meaningfully. 
* Stops scanning after one full sweep left then right.
* COMPLETE - Final challenge for this spiral is to send data that plots according to the bot's heading.
* COMPLETE - Before any measurement data is passed from Arduino to Processing a few things must happen:
*  a. Handshake between the Ard and Proc
*  b. Proc requests plot configuration data from Ard
*  c. Proc configures its plot then requests SCAN and HEADING data from arduino
*/

#include <Servo.h>
#include <math.h>             //Included to get access to PI and arctan
#include <Wire.h>             //I2C Arduino Library

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

// Communication Constants
const byte HEADER = '|';            
const byte CONFIG_TAG = 'c';        
const byte MEASUREMENT_TAG = 'm';
const byte HEADING_TAG = 'h';
const byte TEST_TAG = 't';
const byte EOT = '~';               
boolean commsRequested = false;   
boolean commsComplete = false;      
boolean configRequested = false;    
boolean configComplete = false;     
boolean scanRequested = false;      
boolean scanComplete = false;
boolean headingRequested = false;
boolean coordSendSuccess = false;    
boolean commsGood = false;   

// Span configuration constants
const int sweepWidthInDegrees=180;                        
const int edgeLimit = 15;             //limits servo travel x degrees from limits
const byte segments = 5;               //MUST be a factor of 180.  
const int segmentWidthInDegrees = (sweepWidthInDegrees - (2*edgeLimit))/segments;
const byte observationPoints = segments+1; 

// Plot configuration constants
const int gridSize = 141;           // Must be odd!!  
const int scale = 2;                // Each obs represents a cm^2 SCALE x SCALE brick

// Servo control constants
Servo jackServo;                          
const int jackServoPin=2;   
const int jackServoPause=500;                 //Pause after taking a measurement before moving 
const int jackServoRotationDelay=500;         //allows the servo to rotate before taking a measurement
const int jackServoAdjustmentPotPin = 1;
int jackServoCenter = 90;        
int jackServoOffset;                          
int servoCenterPlusOffset;
int servoStopPoints[observationPoints];
int servoStopPointsIndex = 0;
int pan = 1;                                  //Sign of pan controls sweep direction
int checkpoints = 0;                          //keeps track of scan progress

// PING))) sensor constants
const int pingPin = 7; 

// Test Measurement data
int testData[3] = {154, 36, 2};

void setup() {
	Serial.begin(9600);                  //initialize serial communication
        Wire.begin();                        //initialize I2C bus
        //Put the HMC5883 IC into the correct operating mode
         Wire.beginTransmission(address); //open communication with HMC5883
         Wire.write(0x02); //select mode register
         Wire.write(0x00); //continuous measurement mode
         Wire.endTransmission();
         
	configureServo();                    //attach the servo and find the centering offset
	findStopPoints();                    //find the stop points for the servo
}

void loop() { 
	listen();
	if (commsRequested == true && commsComplete == false) {
		respondToHandshake();
		commsGood = true;
		commsComplete = true;
	}
	if (configRequested == true && configComplete == false) {
		respondToConfig();
		configComplete = true;       
	}
	if (scanRequested==true && scanComplete==false) {
			int theta = servoStopPoints[servoStopPointsIndex];
			long obstructionDistanceCM = scan(theta);
                        int Z = getHeading();
                        delay(jackServoPause);
			int coord[3];                        
			coord[0] = convDisToX(theta, Z, obstructionDistanceCM);
			coord[1] = convDisToY(theta, Z, obstructionDistanceCM);
			coord[2] = findSign(coord[0], coord[1]);
                        coord[0] = abs(coord[0]);
                        coord[1] = abs(coord[1]);
			sendMeasurement(coord[0], coord[1], coord[2]); 
                        
                        //This section of the code finds if the scan is complete
                        int indexMax = observationPoints - 1;
                        if ((checkpoints == 2) && (servoStopPointsIndex == 0)) { 
			    scanComplete = true;
                            rotateToPoint(servoCenterPlusOffset);
                            checkpoints=0;
			}
                        if ((checkpoints == 1) && (servoStopPointsIndex == indexMax)) {
                            checkpoints++;
                        }
                        if (servoStopPointsIndex == 0 && checkpoints == 0) {
                          checkpoints++;
                        }
                        
                        //this section advances the scan index for the next call for a scan 
			advanceServoStopPointsIndex();

                        //this sets the scan request to false after each measurement.  If
                        //Arduino loses comms with Processing the scan will stop because of
                        //this flow control
                        scanRequested = false;
	}
        if (headingRequested==true) {
              respondToHeading();
        }
}

int convDisToX (int theta, int Z, long distance) {  
   	//This is the implementation of the algorithm Hayden discovered for the
        //relationship between Heading - Z, theta - sensor angle,
        //and phi - true bearing of the sensor.	
        //First we have to adjust theta to find the tru angle since we've applied a correction to 
        //account for the mechanical alignment of the servo.
        theta = theta - jackServoOffset;
        float phi = Z - theta;
        phi = phi * M_PI / 180;
	int xCoord = distance * cos(phi);
	return xCoord;
}

int convDisToY (int theta, int Z, long distance) {
   	//This is the implementation of the algorithm Hayden discovered for the
        //relationship between Heading - Z, theta - sensor angle,
        //and phi - true bearing of the sensor.
        //First we have to adjust theta to find the tru angle since we've applied a correction to 
        //account for the mechanical alignment of the servo.
        theta = theta - jackServoOffset;
        float phi = Z - theta;
        phi = phi * M_PI / 180;
   	int yCoord = distance * sin(phi);
   	return yCoord;
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
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }

  //convert to heading
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
  return Z;  
}

void listen() {  
	if (Serial.available() > 0) {
		int inByte = Serial.read(); 
		switch (inByte) {
			case 'A':  //Processing is sending a request to handshake    
				commsRequested = true;
				break;
			case 'c':  //Processing is requesting the scan and plot configuration
				configRequested = true;
				break;
			case 's':  //Processing is requesting a scan
				scanRequested = true;
				break; 
                        case 'h':  //Processing is requesting a heading
                                headingRequested = true;
                                break;
		} 
	} 
}

byte findCheckSum(int x, int y, int sign) {
    int sum = x + y + sign;
    byte remainder = sum % 128;
    return remainder;
}

int findSign(int xCoord, int yCoord) {
	/* Sign is 0-3 and is a binary code for the sign of each coord
	 * 00 is x pos, y pos 
	 * 01 is x pos, y neg
	 * 10 is x neg, y pos
	 * 11 is x neg, y neg */
	int sign=0;
	if(xCoord < 0) {
          sign = sign + 2;
	  xCoord = xCoord * -1;
	}
        if (yCoord < 0) { 
		sign = sign + 1;
		yCoord = yCoord * -1;
	}
	return sign;
}

void advanceServoStopPointsIndex() {
	int indexMax = observationPoints - 1;
        if (servoStopPointsIndex == indexMax) {
                pan = -1;  
        }
        servoStopPointsIndex += pan;
        if (servoStopPointsIndex < 0) {
                pan = 1;
                servoStopPointsIndex = 1; 
        }	        
}

long scan(int angle) {
  	long distanceCM;
	jackServo.write(angle);
	delay(jackServoRotationDelay);               
	distanceCM = measureDistance();
	return distanceCM;
}      

void sendMeasurement(int x, int y, int sign) {  
        byte checkSum = findCheckSum(x, y, sign);
  	Serial.flush();
        Serial.write(HEADER);
	Serial.write(MEASUREMENT_TAG);
	sendBinary(x);
	sendBinary(y);
	sendBinary(sign);
        Serial.write(checkSum);
	Serial.write(EOT);
}

void respondToConfig() {      
        Serial.write(HEADER);
	Serial.write(CONFIG_TAG);
	sendBinary(sweepWidthInDegrees);
	sendBinary(observationPoints);
	sendBinary(gridSize);
	sendBinary(scale);     
	Serial.write(EOT);
}

void respondToHeading() {
        int Z = getHeading();
        byte checkSum = findCheckSum(0, 0, Z);
        Serial.write(HEADER);
	Serial.write(HEADING_TAG);
	sendBinary(Z);
        Serial.write(checkSum);
	Serial.write(EOT);    

        headingRequested = false;
}

void respondToHandshake() {
	Serial.write(1);  		//send a 1 to Processing to handshake
}

void findStopPoints() {  
	int newCenter = 90+jackServoOffset;
//        Serial.println();
//        Serial.print("newCenter is: ");
//        Serial.println(newCenter);
        int startPoint = newCenter - .5 * segments * segmentWidthInDegrees;
//        Serial.print("startPoint is: ");
//        Serial.println(startPoint);        
	for (int i=0; i<observationPoints; i++) {
		int baseAngle = i*segmentWidthInDegrees;
//                Serial.print("     baseAngle is: ");
//                Serial.println(baseAngle);
		baseAngle += startPoint;
		servoStopPoints[i]=baseAngle;
	}  
}

void configureServo() {
  	jackServo.attach(jackServoPin);    
	int val = analogRead(jackServoAdjustmentPotPin);  	// analog read between 0 and 1023
	jackServoOffset = map(val, 0, 1023, -15, 15);     		// scale the pot value
	servoCenterPlusOffset = jackServoCenter + jackServoOffset;
	jackServo.write(servoCenterPlusOffset);           
	int pauseAtCenter = 5*jackServoPause;
	delay(pauseAtCenter);  
}

void rotateToPoint(int theta) {  
	jackServo.write(theta);
        delay(jackServoRotationDelay);
}

void sendBinary(int value) {
  	/* Processing defines bytes from -128 to 128.  Arduino defines bytes as 0 to 256.  
	 * I couldn't get 180 to ship over right.  By changing how the low and high byte it works.
	 */ 	
        byte tempHigh = value/128;
  	byte tempLow = value % 128;
  	if (tempLow == EOT) {
    		tempLow = tempLow-1; //this throws a little innacuracy into the system, but prevents an inadvertant EOT signal
	}
	Serial.write(tempHigh);
        delay(20);
        Serial.write(tempLow); 
	delay(20);
}

long measureDistance() {
  //could be made more accurate with temp and alt variables
  	long duration, cm;
  	// PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  	pinMode(pingPin, OUTPUT);
	digitalWrite(pingPin, LOW);
	delayMicroseconds(2);
	digitalWrite(pingPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(pingPin, LOW);
        //Reading the sensor reply, a HIGH pulse whose duration is the 
	//time (in microseconds) from the sending of the ping to the 
	//reception of its echo off of an object.
	pinMode(pingPin, INPUT);
	duration = pulseIn(pingPin, HIGH);
        // convert the time into a distance
	cm = microsecondsToCentimeters(duration);
	return cm;
}

long microsecondsToCentimeters(long microseconds) {  
	// The speed of sound is 340 m/s or 29 microseconds per centimeter. 
	// The ping travels out and back, so to find the distance of the
	// object we take half of the distance travelled.
	return microseconds / 29 / 2;
}

void showAngularData(int i, int point, long distanceCM){     
	//takes a look at the angular data of the scan on the serial port
	Serial.print("i is: ");
	Serial.print(i);
	Serial.print(" Angle is ");
	Serial.print(point);
	Serial.print(".  Distance is ");
	Serial.println(distanceCM);
}

void showCoords (int x, int y, int sign) {  
	Serial.print ("X is:");
	Serial.print (x);
	Serial.print (" Y is:");
	Serial.print (y);
	Serial.print (" Sign is:");
	Serial.println (sign);
	Serial.println ("----------------------"); 
}

void createTestData() {  
	int bounds = (gridSize-1)/2;
	int tempX = random(0, bounds);
	int tempY = random(0, bounds);
	int tempSign = floor(random(0-4));
	testData[0] = tempX;
	testData[1] = tempY;
	testData[2] = tempSign; 
}

void sendTestMeasurement() { 
	Serial.write(HEADER);  
	Serial.write(MEASUREMENT_TAG);
	for (int i=0; i < sizeof(testData)/sizeof(int); i++) {
		sendBinary(testData[i]); 
	}
	Serial.write(EOT);
}

boolean getCheckSumBackFromProcessing(byte checkSum) {
        boolean messageSentCorrectly = false;	
        byte incomingByte;
        if (Serial.available() > 0) {
           incomingByte = Serial.read();
        }
        if (incomingByte == checkSum) {
             messageSentCorrectly = true;
        }
        return messageSentCorrectly;
}
