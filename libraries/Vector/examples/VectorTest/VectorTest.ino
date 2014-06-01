/* Test my new Vector class.
   The LED will blink a number of time equal to the size of the Vector.
   The values in the two vector elements control how long the LED blinks.
   
   I built this Vector class to give me some of the functionality in the C++11 Vector.
*/

#include <Vector.h>
Vector v(2);            //create the Vector v with 2 elements;

int pinLED = 13;

void setup() {
  v[0] = 500;          //assign values to the Vector elements
  v[1] = 250;
  
  pinMode(pinLED, OUTPUT);
}

void loop() {
  for(int i=0; i!=v.size(); ++i) {
    digitalWrite(pinLED, HIGH);   
    delay(v[0]);               
    digitalWrite(pinLED, LOW);    
    delay(v[1]);              
  }
  delay(3000);
}
