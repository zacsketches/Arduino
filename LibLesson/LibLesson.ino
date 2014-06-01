#include <TestClass.h>

TestClass aTest;

void setup(){
  Serial.begin(9600);
}

void loop() {
  int x = aTest.i;
  Serial.print("aTest.i is: ");
  Serial.println(x);
  aTest.addOne();
  delay(333);
}


