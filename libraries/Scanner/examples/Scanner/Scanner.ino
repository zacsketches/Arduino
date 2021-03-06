/* 
 Scanner implements control over an ultrsonic sensor mounted to a 
 servo in order to scan multiple headings with a single sensor.
 
   The signature for creating a scanner is:
   Scanner(const int servo_pin, 
           const int ping_pin, 
           const int center = 90,
           const int span = 170,
           const int test_points = 5,
           const int servo_angular_rate = 240/60+5);	in millisec / deg.  Futaba S3004 280us/60deg plus a tad
 */
#include <Servo.h>
#include <Scanner.h>

/*--------Global Variables----------*/
Scanner scanner(9, 7, 88, 160, 5, 10);  //jack servo on pin 9 and PING))) on pin 7

void setup() {
  scanner.attach();
  Serial.begin(9600);

  const char* data = scanner.data();
  const char* headings = scanner.headings();

  Serial.println(data);
  Serial.println(headings);

}

void loop() {
  static unsigned long now;
  static unsigned long last_report;
  now = millis();

  scanner.run();    

  //load data and print once per second
  if( now >= (last_report + 1000)) {  //load data and print once per second
    last_report = now;
    const char* data = scanner.data();
    const char* headings = scanner.headings();
    Serial.println(headings);
    Serial.println(data);
  }
}