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
        #if DEBUG_SCAN == 1
          std::cout<<"multiple is: "<< multiple <<std::endl;
        #endif
        #if DEBUG_SCAN == 1
          std::cout<<"dir is: "<< dir <<std::endl;
        #endif
        head = center + (dir * multiple * angular_seperation); 
        elem[i].set_heading(head);      
        #if DEBUG_SCAN == 1
          std::cout<<"head is: "<< head <<std::endl;
        #endif
        if(i==0) ++multiple;
        if(i>0 && (i%2==0)) ++multiple;
        dir = -1 * dir; 
    }
    
    //allocate buffers for the heading character array
    //basic JSON string as follows {\"Headings\": [90,45,135,0,180]}
    //  size:    curlys 2
    //         brackets 2
    //         quotes   2
    //         colon    1
    //        escapes   2
    //       Headings   8
    //       space      1
    //       commas     1 * scan_points...actually scan_points-1 but who's counting
    //        vals      3 * scan points
    //       null term  1
    int heading_buf_sz = 2+2+2+1+2+8+1+(1*scan_points)+(3*scan_points)+1;
    #if DEBUG_SCAN == 1
      std::cout<<"heading_buf_sz is: "<< heading_buf_sz <<std::endl;
    #endif
    heads = new char[heading_buf_sz];
    
    //allocate buffers for the data character array
    //basic JSON string as follows {\"Measurements\": [L,L,L,L,L]}
    //  size:    curlys 2
    //         brackets 2
    //         quotes   2
    //         colon    1
    //        escapes   2
    //    Measurements  12
    //       space      1
    //       commas     1 * scan_points...actually scan_points-1 but who's counting
    //        vals      5 * scan_points  max int is 32,767 (5 digits)
    //       null term  1
    int data_buf_sz = 2+2+2+1+2+12+1+(1*scan_points)+(5*scan_points)+1;
    #if DEBUG_SCAN == 1
      std::cout<<"data_buf_sz is: "<< data_buf_sz <<std::endl;
    #endif
    dat = new char[data_buf_sz];
}

// DESTRUCTOR
Scan::~Scan() {
    //RAII takes care of cleanup when the object goes out of scope
    delete[] elem; 
    delete[] heads; 
    delete[] dat; 

}

// RETURN POINTER TO THE HEADINGS BUFFER CONTAINING A JSON STRING
const char* Scan::headings() const{
    //basic JSON string as follows {\"Headings\": [90,45,135,0,180]}
    
    //temp buffer for string representation of int val
    char buf[sizeof(long)];
    
    strcpy(heads, "{\"Headings\": [" ) ;
    for(int i = 0; i < sz; ++i) {
        //get string representation of heading val
        sprintf(buf, "%d", elem[i].heading());
        
        if(i != 0) strcat(heads,",");
        strcat(heads, buf);
    }
    strcat(heads, "]}");
    
    return heads;
}

// RETURN INT OF A HEADING BY THE INDEX, -1 ON INDEX OUT OF BOUNDS
const int Scan::heading_by_index(const int index) const{
    if( (index >=0) && (index < sz) ) {
        return elem[index].heading();
    }
    return -1;
}


// RETURN POINTER TO THE DATA BUFFER CONTAINING A JSON STRING
const char* Scan::data() const{
    //basic JSON string as follows {\"Measurements\": [L,L,L,L,L]}
    
    //temp buffer for string representation of long val
    char buf[sizeof(long)];
    
    strcpy(dat, "{\"Measurements\": [");
    for(int i = 0; i < sz; ++i) {
        //get string representation of dat val
        sprintf(buf, "%d", elem[i].data());
        
        if(i != 0) strcat(dat, ",");
        strcat(dat, buf);
    }
    strcat(dat, "]}");
    
    return dat;
}

// UPDATE DATA IN THE SCAN
bool Scan::update_by_heading(const int heading, const int data){
    //returns true on successful update
    bool success = false;
    for(int i = 0; i < sz; ++i) {
        //find index for given heading
        if (elem[i].heading() == heading) {
            elem[i].set_data(data);
            success = true;
            break;
        }
    }
    return success;
}

bool Scan::update_by_index(const int index, const int data){
    //returns true on successful update
    bool success = false;
    if( (index >=0) && (index < sz) ) {
        elem[index].set_data(data);
        success = true;
    }
    return success;
}

//*******************************************************************
//*                         SCAN ORDER CONSTRUCTOR
//*******************************************************************
Scanner::Scan_order::Scan_order(int test_points) {
    pos = 0;
    sz = 2* (test_points -1);
    order = new int[sz];
    int mult = 1;
    for(int i = 0; i < sz; ++i) {
        if(i%2 == 0) order[i] = 0;    //always come back to the middle
        else {
            order[i] = mult;
            ++mult;
        }
    }     
}


//*******************************************************************
//*                         SCANNER CLASS METHODS
//*******************************************************************

// CONSTRUCTOR
Scanner::Scanner(const int servo_pin, 
    const int ping_pin, 
    const int center,
    const int span,
    const int test_points,
    const int servo_angular_rate)
    :sp(servo_pin), pp(ping_pin), ctr(center), scan(test_points, span, center),
     sar(servo_angular_rate), scan_order(test_points)
{
    //attach servo
    servo.attach(servo_pin);
    servo_state = 0x01;     // Ready
    
    /*
        TODO take the hard coded cheats out of scan_order
    */
    
    //no setup configuration required for Ping))) sensor
}

// RUN ... take data and store it.
void Scanner::run(){
    static unsigned long command_time;	//time servo ordered to move
    static unsigned long ready_time;	//time servo will be ready
    
    //if servo is ready then order next scan
    if(servo_state & B0001 == 0x01) {	    //B0001 tests servo_ready bit
        int target_heading = scan.heading_by_index( scan_order.current() );
        command_time=millis();		        //record time move was ordered
        ready_time = command_time + find_delay(target_heading);
        servo.write(target_heading);		//order servo to move
        servo_state = 0x02;		           //set state to B0000 0010->move ordered
        #if DEBUG_SER == 1
            Serial.print("\tcommand time is: ");
            Serial.println(command_time);
            Serial.print("\tready time is: ");
            Serial.println(ready_time);
            Serial.print("\tservo state is: ");
            Serial.println(servo_state, HEX);
        #endif          
    }
    
    //if servo move ordered and time has elapsed then move is complete
    if((servo_state == 0x02) && ( millis()>=ready_time) ) { //B0000 0010->move_ordered
        servo_state = 0x04; 	//B0000 0100->move_complete
    }
    
    //if servo move complete, then pulse and update measurement
    if(servo_state == 0x04) {	//B0000 0100->move_complete
        take_reading();
        servo_state = 0x01;	    //B0000 0001->ready
        ++scan_order;           //advance current to the the next scan point
    }
}

//*******************************************************************
//*                         SCANNER HELPER FUNCTIONS
//*******************************************************************

// RETURN DELAY IN MILLISECONDS FOR THE SERVO TO MOVE BETWEEN TWO ANGLES
int Scanner::find_delay(const int target_angle) {
	int current_angle = servo.read();
	int theta = target_angle - current_angle;
	theta = abs(theta);

	int delay = theta * sar;
	return delay;	//in milliseconds
}

void Scanner::take_reading() {
    //the servo is already in the right position when this method is
    //called so we just need to bang the sonar, get the measurement
    //and then save it to the scan
    long dur = pulse();
    int cm = us_to_cm(dur);
    
    int scan_index = scan_order.current();
    scan.update_by_index(scan_index, cm);
}

// PULSE THE SENSOR AND STORE THE DATA
long Scanner::pulse() {
    //standard pulse method for Parallax PING))) sensor
    pinMode(pp, OUTPUT);
    digitalWrite(pp, LOW);
    delayMicroseconds(2);
    digitalWrite(pp, HIGH);
    delayMicroseconds(5);
    digitalWrite(pp, LOW);

    // The same pin is used to read the signal from the PING))): a HIGH
    // pulse whose duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(pp, INPUT);
    long duration = pulseIn(pp, HIGH);

    return duration;
}


int Scanner::us_to_cm(const long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  /*
    TODO I could calibrate this and take temp and pressure into account,
    but this doesn't fix the lack or resolution in the sonar sensor
  */
  return microseconds / 29 / 2;
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


*/