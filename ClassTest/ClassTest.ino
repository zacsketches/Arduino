# Class_Test.h

test aTest = test();

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("aTest.i: ");
  Serial.println(aTest.i);
  aTest.addOne();
  delay(500);
}
