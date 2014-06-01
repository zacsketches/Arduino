/* Written on 3/11/2013 to get a system to compute and evaluate checksums between Processing
 * and Arduino
 */
 
// Communication Constants
const byte HEADER = '|';            
const byte CONFIG_TAG = 'c';        
const byte MEASUREMENT_TAG = 'm';   
const byte TEST_TAG = 't';
const byte EOT = '~';               
boolean commsRequested = false;   
boolean commsComplete = false;      
boolean configRequested = false;    
boolean configComplete = false;     
boolean scanRequested = false;      
boolean scanComplete = false;
boolean scanSuccess = false;    
boolean commsGood = false;

//test constants
const int blinkPin = 13;

// Test Measurement data
int testData[3] = {154, 36, 2};

void setup() {
	Serial.begin(9600);                  //initialize serial communication
        pinMode(blinkPin, OUTPUT);     
        
}

void loop() {
  boolean test = sendMeasurement(25, 50, 0);
  if (test) {
    for (int i=0; i<5; i++) {
      digitalWrite(blinkPin, HIGH);
      delay(200);
      digitalWrite(blinkPin, LOW);
      delay(200);
    }
  }
  delay(4000);
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

/* I used to use a control variable ProcessingReady and then measurements just started coming...
 * Now I want to request the scan and control how it's doing.
			case 'r':  //Processing is sending that it is configured and ready to receive measurement data
				processingReady = true;
				break;
*/
			case 's':  //Processing is requesting a scan
				scanRequested = true;
				break; 
		} 
	} 
}

boolean sendMeasurement(int x, int y, int sign) {  
        byte checkSum = findCheckSum(x, y, sign);
  	Serial.flush();
        Serial.write(HEADER);
	Serial.write(MEASUREMENT_TAG);
	sendBinary(x);
	sendBinary(y);
	sendBinary(sign);
        Serial.write(checkSum);
	Serial.write(EOT);

	boolean commSuccess = checkConnection(checkSum);
        return commSuccess;
}

byte findCheckSum(int x, int y, int sign) {
    int sum = x + y + sign;
    byte remainder = sum % 128;
    return remainder;
}

boolean checkConnection(byte checkSum) {
        boolean success = false;
        int incomingByte;	
        if (Serial.available() > 0) {
           incomingByte = Serial.read();
        }
        if (incomingByte == checkSum) {
             success = true;
        }
        return success;
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
