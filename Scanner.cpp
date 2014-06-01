/*
    TODO add my beer license
*/
/* 
 
 A servo is activated by creating an instance of the Servo class passing the desired pin to the attach() method.
 The servos are pulsed in the background using the value most recently written using the write() method
 
 Note that analogWrite of PWM on pins associated with the timer are disabled when the first servo is attached.
 Timers are seized as needed in groups of 12 servos - 24 servos use two timers, 48 servos will use four.
 
 The methods are:
 
 Servo - Class for manipulating servo motors connected to Arduino pins.
 
 attach(pin )  - Attaches a servo motor to an i/o pin.
 attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
 default min is 544, max is 2400  
 
 write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
 writeMicroseconds() - Sets the servo pulse width in microseconds 
 read()      - Gets the last written servo pulse width as an angle between 0 and 180. 
 readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
 attached()  - Returns true if there is a servo attached. 
 detach()    - Stops an attached servos from pulsing its i/o pin. 
 
*/

#include "Scanner.h"


//*******************************************************************
//*                         SCAN METHODS
//*******************************************************************

// CONSTRUCTOR
/*
    TODO implement margin near the edges of the servo travel
*/
Scan::Scan(const int scan_points, const int span, const int center) 
    :sz(scan_points), spn(span), elem(new Scan_point[scan_points])
{
    //divide the span in angular sections and assign the test points
    int sections = scan_points - 1;
    int angular_seperation = span / sections;
    
    //scan point zero is at the center of the scan with even points
    //to the left (port) and odd points to right (starboard)...can't
    //help it..Navy habits die hard
    int head = center;
    int dir = -1;
    int multiple = 0;
    for(int i = 0; i < scan_points; ++i) {
        if(DEBUG_SCAN) std::cout<<"multiple is: "<< multiple <<std::endl;
        if(DEBUG_SCAN) std::cout<<"dir is: "<< dir <<std::endl;
        head = center + (dir * multiple * angular_seperation); 
        elem[i].set_heading(head);      
        if(DEBUG_SCAN) std::cout<<"head is: "<< head <<std::endl;
        if(i==0) ++multiple;
        if(i>0 && (i%2==0)) ++multiple;
        dir = -1 * dir; 
    }
    
    //allocate buffers for the heading character array
    //basic JSON string as follows {"Headings": [90,45,135,0,180]}
    //  size:    curlys 2
    //         brackets 2
    //         quotes   2
    //         colon    1
    //       Headings   8
    //       space      1
    //       commas     1 * scan_points...actually scan_points-1 but who's counting
    //        vals      3 * scan points
    //       null term  1
    int heading_buf_sz = 2+2+2+1+8+1+(1*scan_points)+(3*scan_points)+1;
    if(DEBUG_SCAN) std::cout<<"heading_buf_sz is: "<< heading_buf_sz <<std::endl;
    heads = new char[heading_buf_sz];
    
    //allocate buffers for the data character array
    //basic JSON string as follows {"Measurements": [L,L,L,L,L]}
    //  size:    curlys 2
    //         brackets 2
    //         quotes   2
    //         colon    1
    //    Measurements  12
    //       space      1
    //       commas     1 * scan_points...actually scan_points-1 but who's counting
    //        vals      size_of(long) * scan_points
    //       null term  1
    int data_buf_sz = 2+2+2+1+12+1+(1*scan_points)+(sizeof(long)*scan_points)+1;
    if(DEBUG_SCAN) std::cout<<"data_buf_sz is: "<< data_buf_sz <<std::endl;
    dat = new char[heading_buf_sz];
}

// DESTRUCTOR
Scan::~Scan() {
    //RAII takes care of cleanup when the object goes out of scope
    delete[] elem; 
    delete[] heads; 
    delete[] dat; 

}


/*

long Scanner::measure_angle(int h) {
	double travel_delay = find_delay(h);
	travel_delay = int(travel_delay);
	this->write(h);
	delay(travel_delay);
    long duration = this->pulse(ping_pin);
    long cm = microsecondsToCentimeters(duration);
    delay(pulse_delay);
	return cm;
}

long Scanner::microsecondsToCentimeters(const long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

long Scanner::pulse(const int ping_pin) {
  pinMode(ping_pin, OUTPUT);
  digitalWrite(ping_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(ping_pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(ping_pin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(ping_pin, INPUT);
  long duration = pulseIn(ping_pin, HIGH);
  
  return duration;
}

double Scanner::set_servo_angular_rate(double new_rate) {
	servo_angular_rate = new_rate;
	return get_servo_angular_rate();
}

double Scanner::get_servo_angular_rate() {
	return servo_angular_rate;
}

double Scanner::find_delay(int target_angle) {
	int current_angle = this->read();
	int diff = target_angle - current_angle;
	diff = abs(diff);

	double delay = diff * get_servo_angular_rate();
	return delay;	
}
*/