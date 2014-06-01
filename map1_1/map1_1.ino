//Written by Greyson, Hayden and Dad in Feb 2013
//This is the initial attempt at building the array of sensor data
//Ping))) sensor code adapted from work of David A. Mellis and Tom Igoe
//This code prevents driving the jackServo all the way to 180 and 0 degrees.
//Segments must be 6 or greater.  180 divided by segments must be an integer

/*As of 16 Feb, this 1.1 version successfully builds measurementArray[] and 
  measurementIndex[].  It successfully scans and measures, but does not yet
  store those measurements to the measurementArray[]
*/

#include <Servo.h>
Servo jackServo; //makes the jackServo available globally
const int servoPin=2; //where is the servo plugged in
const int servoSafetyDelay=2000; //protects the servo from getting commands too quickly
const int servoRotationDelay=500; //allows the servo to rotate before taking a measurement

const byte span=180; //how wide is the sweep in degrees
const byte segments=6; //how many segments will the span be divided into
const byte observationPoints = segments+1; //finds how many observation points there are

const byte arrayColumns = observationPoints - 4; //finds the array width to hold measurement data



/* Config for the Ping))) Sensor
	* +V connection of the PING))) attached to +5V
	* GND connection of the PING))) attached to ground
	* SIG connection of the PING))) attached to digital pin 7
 */
const int pingPin = 7; //where the PING))) is plugged in

void setup() {
  
  // initialize serial communication:
  Serial.begin(9600);
  
  //get the servo set up
   jackServo.attach(servoPin);
   swingRight();
   
   //Set up the matrix to store sensor measurement data with zeros in each element AND
   //set up the index of how to locate each array location.
   int measurementArray[3][arrayColumns];  //This is where distance measurements will be stored
   int measurementIndex[observationPoints][3];  //This is the address table for the measurements
   byte pointer=0;  //pointer for the measurementIndex
   
   for (int j=arrayColumns-1; j>=0; j--) {
      for (int i=2; i>=0; i--) { 
         measurementArray[i][j]=0;
         
         //test to see if the Array address belongs in the index
         //recall that we only use the right, top and left edge of the array   
         //right edge
         if (j==arrayColumns-1) {
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         //top row
         else if (i==0){
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         //left edge
         else if (j==0){
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         
         // This section prints the array for testing.
         /*
         if(j==arrayColumns-1 && i==2) {
         Serial.println("Walk through the Measurement Array: "); 
         }
         Serial.print("Element [");
         Serial.print(i);
         Serial.print("][");
         Serial.print(j);
         Serial.print("] is: ");
         Serial.println(measurementArray[i][j]);      
         */     
      } 
   }
   
   //When you view the last bit of code it has the last three values in the index in the wrong order
   //this next bit swaps the last and 2nd to last elements to get them right.
   int lastElement = observationPoints-1;
   int secondToLastElement = observationPoints-3;
   int tempArray1[] = {measurementIndex[lastElement][0], measurementIndex[lastElement][1]}; 
   int tempArray2[] = {measurementIndex[secondToLastElement][0], measurementIndex[secondToLastElement][1]};
   //swap the last element
   measurementIndex[lastElement][0]=tempArray2[0];
   measurementIndex[lastElement][1]=tempArray2[1];
   //swap the 2nd to last element
   measurementIndex[secondToLastElement][0]=tempArray1[0];
   measurementIndex[secondToLastElement][1]=tempArray1[1];
   
   //Send the contents of the Measurement Index to the serial port for a look
   /*
   Serial.println("Walk through the Measurment Index");
   for(int i=0; i<observationPoints; i++){
      Serial.print("Index [");
      Serial.print(i);
      Serial.print("] is: ");     
      for(int j=0; j<2; j++) {
         Serial.print(measurementIndex[i][j]);
         Serial.print(" ");
      }
      Serial.println();
   }
   */
   
}

void loop() {
  scan();  
  
}

void scan() {
  int segment = (span/segments); //finds the angular width of the segment
  int pan=1; //used to control sweep direction
  
  long distanceCM; //stores the distance measurement in cm
  
  for (int i=1; i>-1; i=i+pan) {
     int point = abs(i)*segment;
     if (point>=180) pan=-1;  //this reverses the pan direction
     point = constrain(point, 5, 175);  //protects the edges of the servo rotation
     rotateToPoint(point);
    
     delay(servoRotationDelay); //to allow time for the servo to rotate
    
     distanceCM = measureDistance();
     //Print the rotation angle and distance to the serial port
     Serial.print("i is: ");
     Serial.print(i);
     Serial.print(" Angle is ");
     Serial.print(point);
     Serial.print(".  Distance is ");
     Serial.println(distanceCM);
   
     delay(servoSafetyDelay); //pause between measurements
  }
  
  Serial.println("Out of the scan");
}


long measureDistance() {
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, cm;
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void swingRight() {
  jackServo.write(0);
  delay(servoSafetyDelay);
}

void rotateToPoint(int point) {
  jackServo.write(point);
}
