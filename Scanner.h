
/* 
  A scanner is activated by creating an instance of the Scanner class then passing
  passing the desired servo connection pin and the PING))) sensor pin to the attach() function.
  The servos are pulsed in the background using the value most recently written using the write() method

  Note that analogWrite of PWM on pins associated with the timer are disabled when the first servo is attached.
  Timers are seized as needed in groups of 12 servos - 24 servos use two timers, 48 servos will use four.
  The sequence used to sieze timers is defined in timers.h

  The methods are:

   Scanner - Class for manipulating servo motors connected to Arduino pins.

   attach(servo_pin, ping_pin )  - Attaches a servo motor and a PING))) sensor to an i/o pin
 
   write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
   writeMicroseconds() - Sets the servo pulse width in microseconds 
   read()      - Gets the last written servo pulse width as an angle between 0 and 180. 
   readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
   attached()  - Returns true if there is a servo attached. 
   detach()    - Stops an attached servos from pulsing its i/o pin. 
 */

/* 7/24/13 Added a bit of code to calculate the degrees of travel and then the amount of 
   delay required for the servo to move to this position.

   NOTE: use of double type for calculating delay does not increase precision on 
   Arduino Uno, since double is a 4 byte type just like float.  I use double for
   consistency in implementing C++ floating point numbers as unless there
   are memory optimizations required to use other types.
*/

#ifndef Scanner_h
#define Scanner_h

#include "Servo.h"

#define DEBUG_SCAN 1

#if DEBUG_SCAN == 1
    #include <iostream>
#endif

class Scan_point{
 	int h;		      //heading in degrees...nominal range if [0 : 180]
 	int elem;        //most recent reading at this point, in cm for ultrasonic sensor
public:
  Scan_point(const int heading=90, const int data = 0)
    :h(heading), elem(data) {}
   	
  int heading() const { return h; }
  void set_heading(int head) { h = head; }

  const int data() const { return elem; }
  void set_data( int dat ){ elem = dat; }
};

class Scan {
  int sz;                         //number of Scan_points in the scan
  int spn;                       //degrees to scan (constrained by servo max physical limit)
  Scan_point* elem;             //array of length sp of Scan_points
  char* heads;
  char* dat;

public:  
  Scan(const int scan_points=5, const int span=180, const int center=90);
  ~Scan();
  
  // NON-MODIFYING METHODS //
  int size() const { return sz; }
  int span() const { return spn; }
  const char* headings() const;        //JSON format pointer to heads[]
  const char* data() const;            //JSON format pointer to dat[]
  
  // MODIFYING METHODS //            
  bool update_by_heading(const int heading, const int data);  //returns true on successful update
  bool update_by_index(const int index, const int data);
  
};

/* commented out so I can test my data structures first

class Scanner {
private: 
  //Servo data
  int sp;               //Servo pin
  int ctr;              //where the servo points directly ahead of the bot
  Servo servo;          //Servo object

  //Ultrasonic sensor data
  int pp;               //ping_pin

  //Scan object handles data for the scanner
  Scan  scan;           

public:
  //constructor
  Scanner(const int servo_pin, 
          const int ping_pin, 
          const int center = 90,
          const int span = 180,
          const int test_points = 5);
  
  char[] headings();

  long measure_angle(int heading);    // return distance measurement at angle provided
  void detach();
  void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
  void writeMicroseconds(int value); // Write pulse width in microseconds 
  int read();                        // returns current pulse width as an angle between 0 and 180 degrees
  int readMicroseconds();            // returns current pulse width in microseconds for this servo
  bool attached();                   // return true if this servo is attached, otherwise false
  double set_servo_angular_rate(double new_rate);	//return newly set rate.
  double get_servo_angular_rate(); 
  long pulse(const int ping_pin);	 	// ping the sensor and return the distance in cm
  long microsecondsToCentimeters(const long microseconds);	//convert pulse duration to distance
private:
  double find_delay(int target_angle); 	//compute delay for servo to travel to target heading
  int scan_points[5];					//array of angles measurements are taken at
  int scan_delay;						  //milliseconds
  int pulse_delay;              		//milliseconds
  double servo_angular_rate;		    //Seconds per degree.
 
  uint8_t servoIndex;               // index into the channel data for this servo
  int8_t min;                       // minimum is this value times 4 added to MIN_PULSE_WIDTH    
  int8_t max;                       // maximum is this value times 4 added to MAX_PULSE_WIDTH   
};

*/

#endif
