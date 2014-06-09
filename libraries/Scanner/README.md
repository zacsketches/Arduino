Arduino Scanner
===============

Scanner implements control over an ultrsonic sensor mounted to a  servo in order to scan multiple headings with a single sensor.
      
Installation:
--------------

1.  The easiest way to use this library is to change into the arduino libraries folder at a command prompt (terminal app on a Mac).  On my Mac installation this folder is at

        /Users/user_name/Documents/Arduino/libraries

2.  Once in the libraries folders use git to clone a copy of this repository

        $: git clone https://github.com/zacsketches/Scanner.git
    
3.  That's it...pretty easy.  Now when you restart the Arduino IDE, look under examples/Scanner and you can load the example provided.

Usage:
-----------

####Minimal Usage

The minimal usage takes advantage of a default configured scanner where the servo points the ping sensor directly ahead of the robot when commanded to the 90 degree position.  It scans 170 degrees (85 degrees either side of center) and divides this span into 5 test points.  The first test point is straight ahead with two test points to the right and two to the left of center.  The scanner also times the pings based on how fast the servo travels.  The default is to allow the servo 9ms per degree of travel to get into position before the sonar sensor pings.  This servo angular rate is a generous estimate based on the Futaba S3004 hobby servo which takes about 280ms to travel 60 degrees with a 6V supply.  I usually drive my servos on 5V so I add a cushion of 5ms per degree.  With these defaults the simplist usage is:

````C++
#include <Scanner.h>
#include <Servo.h>

const int servo_pin = 3;      // This pin will control the servo angle and must be one othe PWM capable pins.
                              // On an Arduino UNO the pwm pins are 3, 5, 6, 9, 10, and 11.

const int ping_pin = 2;       // On the Parallax sonar sensor I prefer there are only three connections.  
                              // +5V, Gnd, and the signal pin.  The signal pin is used to trigger the sonar
                              // and to measure the pulse duration.  See the arduino playground for more on this
                              // sensor at http://arduino.cc/en/Tutorial/ping
                              
Scanner scanner(servo_pin, ping_pin);

void setup() {
      Serial.begin(9600);
      scanner.attach();
}

void loop() {

      scanner.run();                // Must be in the loop because it updates the scan every time its called.

      char* data = scanner.data();  // Get most current data from the scanner in a JSON formatted string.  This 
                                    // library uses a JSON format so you can post the data to a web service or
                                    // an other web-enabled usage you need.  To parse the JSON string in the
                                    // Arduino sketch use the JSON parser here:
                                    // https://github.com/bblanchon/ArduinoJsonParser.git
                                    
      Serial.println(data);
}

````

####The signature for creating a scanner where you can configure all its properties is:

```c++
   Scanner(const int servo_pin, 
           const int ping_pin, 
           const int center = 90,
           const int span = 170,
           const int test_points = 5,
           const int servo_angular_rate = 240/60+5);	//in millisec / deg.  Futaba S3004 280ms/60deg
```
#### Some other important things to keep in mind
  * Calling `.data()` or `.headings()` returns a JSON formatted string of the most recent data in the scanner.
  * You must run `.attach()` in the setup method of the Arduino sketch.
  * The `.run()` method will start the scanner.  
  * Measurements are in cm.
  * Data storage is handled by the class with RAII principles.  No need for users to call `delete`.
