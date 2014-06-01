/* Written by Dad on 15 Feb, 2013 to drive a servo off serial input
   in order to work out the range of motion.  This little sketch gives
   me the ability to move the servo around while I bind up the wires
   and make sure the mechanical setup is going to work.
   
   NOTE: the function atoi(buffer) comes from the C++ standard libraries (stdlib.h)
   which are included automatically in all sketches.  See more about standard
   libraries at http://www.cplusplus.com/reference/clibrary/cstdlib
   Additionally, the vast majority of the functions from the cstring, cstdio,
   math, and cctype are included.
*/
#include <Servo.h>

byte incomingByte=0;
char incomingBytes[3];
int servoPos = 0;

Servo testServo;

void setup() {
 testServo.attach(2);
 Serial.begin(9600);
 Serial.println("Type a three character value between 000 and 180");
}

void loop() {
	// send data only when you receive data:
	if (Serial.available() > 0) {
		// reads a single incoming byte:
		//incomingByte = Serial.read();

                //reads a string of 3 bytes:
                Serial.readBytes(incomingBytes, 3);

		// say what you got...used this to figure out how to manage the serial port.
                // commented it out whenI had it working.
		//Serial.print("I received: ");
		//Serial.write(incomingBytes);  //will display the ASCII version of the input
                //Serial.print(incomingByte);  //will display the ASCII code of the input
                //Serial.println();
                
                servoPos = atoi(incomingBytes);  //convert the bytes to an integer
                
                servoPos = constrain(servoPos, 0, 180);  //protects the servo limits
                
                Serial.print("The Servo Position is ordered to: ");
                Serial.println(servoPos);
                
                testServo.write(servoPos); //move the servo
	}
}
