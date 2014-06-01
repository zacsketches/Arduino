#include <Scanner.h>
#include <Servo.h>
 
Scanner scanner;

void setup() {
  Serial.begin(9600);
  delay(300);
  scanner.attach(9,8);
  int test_pin = scanner.get_attach_pin();
  Serial.print("The servo is attached on pin: ");
  Serial.println(test_pin);
  
  //see scan_points
  for(int i=0; i<5; i++) {
    int tp = scanner.get_test_point(i);
    Serial.print("The test point is: ");
    Serial.println(tp);
  }
  
}

void loop() {
  delay(1000);
  scanner.drive_servo_to(130);
  delay(250);
  int pos = scanner.read_servo();
  Serial.print("The last order was: ");
  Serial.println(pos); 
  
  delay(1000);
  scanner.drive_servo_to(50);
  delay(250);
  pos = scanner.read_servo();
  Serial.print("The last order was: ");
  Serial.println(pos);
}
