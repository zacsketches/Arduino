/*This was my first attempt at building a 
 * library.  This library does very little
 * that's useful..just one method to 
 * add onto the private variable _i.
 */
#include <TestClass.h>

TestClass aTest;

void setup(){
  Serial.begin(9600);
}

void loop() {
  int x = aTest.getI();
  Serial.print("aTest.i is: ");
  Serial.println(x);
  aTest.addOne();
  delay(333);
}


