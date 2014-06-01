/* Alfred5 adds a data logger on A1....this logger records anything sent to Serial.print.
   Alfred5 also includes bump detection by sensing motor current.

   Alfred4 takes the functioning system and tries to optimize the scan and motor control
   vectors to better avoid objects.  Also gets rid of hard coded addresses for the 
   center scan.

   Alfred3 incorporates my lessons learned from scanner_ex2 where I worked out the code for 
   a servo that varies its delay based on the distance required to travel and includes
   an interrupt button to increse the delay multiple if the robot is acting "weird".
*/
#include <Scanner.h>
#include <Bounce.h>
#include <Rover5Motor2.h>

/*----Data structures--------*/
struct Scan_point{
	int index;		//1 to number_of_scan_headings
	int heading;		//in degrees
	long trip_distance;	//distance the sensor trips
	byte mask_value;	//1, 2, 4, 8, 16,	
	long current_distance;  //most recent reading at this point
};
struct Control_vec{
	char label[13];
        int m1_dir;
	int m1_pwm;
	int m2_dir;
	int m2_pwm;
	bool overwrite;		//true if the control_vec is to completely overwrite the existing vector.
};

/*------Constants------------*/
const int number_of_scan_headings = 5;
const int number_of_scan_stops = 8;
const int scan_order[] = {2,0,1,0,4,0,3,0};   //compared to Scan_point.index
const int* center = &scan_order[1];
const int servo_pin = 9;
const int ping_pin = 8;
const int mtr_rt_current_pin = 1;
const int mtr_lt_current_pin = 2;
const int stall_clear_time = 1000;  //milliseconds.  Robot will follow stall protocal for this time.
const float stall_current = .90;                //based off config runs on 11Aug with 7.5V Alkaline battery pack.
const double base_angular_rate = .28/60+.0005;	//Futaba S3004 .28sec/60deg plus a tad from an earlier config run

/*------Motor Control Constants-----------------*/
const Control_vec forward = {"forward", 0, 100, 0, 100, true};
const Control_vec backward = {"backward", 1, 100, 1, 150, true};
const Control_vec ease_right = {"ease_right", 0, 100, 0, 70, true};
const Control_vec ease_left = {"ease_left", 0, 70, 0, 100, true};
const Control_vec swerve_right = {"swerve_right", 0, 30, 0, 120, true};
const Control_vec swerve_left = {"swerve_left", 0, 120, 0, 30, true};
const Control_vec twist_right = {"twist_right", 1, 100, 0, 100, true};
const int number_of_controls = 7;

/*-----Interrupt config------*/
// Button attached on pin 4 is debounced via the Bounce.h library and the debounced
// output drives an interrupt on pin 2.  Button is wired into pin 4 with the internal
// pull-up resisitor enabled so depressing the button gives a falling edge.  
// Interrupt 0 is set for a falling edge and serviced by a routine that increments
// a multiple that is then added to the servo angular rate.  This allows me to slowly increase
// the speed of the servo rotation to find an optimal speed base on the servo's voltage and 
// mechanical capabilities.
const int int_pin = 2;
const int button_pin = 12;
Bounce button = Bounce(button_pin, 5);
volatile int scaler = 0;
const double multiple = .0005;

/*--------Global Variables----------*/
double servo_angular_rate = 0;	
Scan_point scan[number_of_scan_headings];
const int* order_pointer = scan_order;
Scanner scanner;  			//requires attach function in setup
byte sensor_state = 0x00;		//five least sig bit represent results from each scan point
byte servo_state = 0x00;		//B0001->servo ready 
					//B0010->move ordered 
					//B0100->move complete
					//B1000->sensor_state updated
boolean stall = false;                  //set by current sensing on the motors
Control_vec controls[number_of_controls];  //array initialized in setup
Rover5Motor2 mtr_rt = Rover5Motor2(5,4);   //creates an intance of my motor class for each motor
Rover5Motor2 mtr_lt = Rover5Motor2(6,7);

/*-------Flow control flags---------*/
boolean scan_complete = false;

/*------Configuration Options--------*/
const bool debug = false;
const bool debug_sensor_state = true;
const bool debug_scan_results = false;
const bool use_button = false;
const bool debug_motor_current = true;

void setup() {
  Serial.begin(9600);
  
  if(use_button) {
    attachInterrupt(0, ISR_increase_scaler, FALLING);	//INT0 is on I/O 2 for arduino Uno
    pinMode(button_pin, INPUT_PULLUP);
    pinMode(int_pin, OUTPUT);
  }

  //configure scan points
  Scan_point p1 = {3, 170, 30, B10000};
  Scan_point p2 = {1, 130, 60, B01000};
  Scan_point p3 = {0, 90, 60,  B00100};  //center 
  Scan_point p4 = {2, 50, 60,  B00010};
  Scan_point p5 = {4, 10, 30,  B00001};

  //push the scan points into the scan array
  scan[0] = p1;
  scan[1] = p2;
  scan[2] = p3;
  scan[3] = p4;
  scan[4] = p5;
  
  //push the control vectors into the array
  controls[0] = forward;
  controls[1] = backward;
  controls[2] = ease_right;
  controls[3] = ease_left;
  controls[4] = swerve_right;
  controls[5] = swerve_left;
  controls[6] = twist_right;

  //configure scanner
  scanner.attach(servo_pin, ping_pin);
  if(scanner.attached()) {
	servo_state |= B0001;	//set servo_state to ready
	char buffer[75];
	sprintf(buffer, "Scanner servo attached on pin %d \nScanner sensor attached on pin %d", servo_pin, ping_pin);
	Serial.println(buffer);
	Serial.println("--------------------------------------");
        scanner.write(90);
  } else {
  	//TODO: throw error for scanner failed to attac
  }
}

void loop() {
  static unsigned long command_time;	//time servo ordered to move
  static unsigned long ready_time;	//time servo will be ready
  static unsigned long stall_time;      //time of stall
  static unsigned long stall_clear;     //time stall will clear and return to normal drive algorithm
  static long cm;
  
  //updating the debounced button and writing the output to the interrupt pin
  button.update();
  digitalWrite(int_pin, button.read());
  
  //TODO: make this a constant in production version
  servo_angular_rate = base_angular_rate + (scaler*multiple);

  //if servo is ready then order next scan
  if(servo_state & B0001 == 0x01) {	//B0001 tests servo_ready bit
	order_pointer = advance_order_pointer(sensor_state);	
  	int scan_index = find_scan_target(*order_pointer);
	int heading = get_heading(scan_index);
	command_time=millis();		//record time move was ordered
	ready_time = find_ready_time(heading, command_time, servo_angular_rate);
	scanner.write(heading);		//order servo to move
	servo_state = B0010;		//set state to B0000 0010->move ordered
        if(debug) {
          Serial.print("\tcommand time is: ");
          Serial.println(command_time);
          Serial.print("\tready time is: ");
          Serial.println(ready_time);
          Serial.print("\tservo state is: ");
          Serial.println(servo_state, HEX);
        }
  }

  //if servo move ordered and time has elapsed for move then pulse
  if((servo_state == B0010) && ( millis()>=ready_time) ) {
	long duration = scanner.pulse(ping_pin);
	cm = scanner.microsecondsToCentimeters(duration);
	servo_state = B0100; 	//B0100->move_complete
  }

  //if servo move complete, compare measurement against trip distance and update sensor_state	
  if(servo_state == B0100) {	//B0100->move_complete
  	int scan_index = find_scan_target(*order_pointer);
	Scan_point s = scan[scan_index];
	if(cm <= s.trip_distance) {
		sensor_state |= s.mask_value;	//set the bit
	} else {
		sensor_state &= ~s.mask_value;	//clear the bit
	}
	servo_state = B1000;	//B1000->sensor_state updated
        if(debug_sensor_state) {
          char buffer[50];
          sprintf(buffer, "\tThe sensor_state is: %#x", sensor_state);
          Serial.println(buffer); 
        }
  }

  //if sensor_state updated, record distance and put servo back at the ready
  if(servo_state == B1000) {	//B1000->sensor_state updated
	int scan_index = find_scan_target(*order_pointer);
	scan[scan_index].current_distance = cm;
	servo_state = B0001; 		//B0001->servo_ready;
	if(this_is_last_scan_point(order_pointer)) {
		scan_complete = true;
	}	
  }

  //check for stall
  //config data of 11 Aug on 7.5V Alkaline battery pack showed stall current was about 1A on each channel.
  //Motor controller returns approx 1V per 1A of current on analog current pin
  float mtr1_current = analogRead(mtr_rt_current_pin) * .0049;  //.0049 is based on 1024 steps over 5V.  
  float mtr2_current = analogRead(mtr_lt_current_pin) * .0049;
  if(mtr1_current > stall_current || mtr2_current > stall_current) {
    stall = true;
    stall_time = millis();
    stall_clear = stall_time + stall_clear_time;
  }
  if(millis() > stall_clear) 
    stall = false;

  //update control vector and drive bot
  int control_index = update_control(sensor_state, stall);
  drive(control_index);

  //compute motor current and log
  
  if(debug_motor_current) {
    Serial.println(millis());
    Serial.print("\trt mtr current: ");
    Serial.println(mtr1_current);
    Serial.print("\tlt mtr current: ");
    Serial.println(mtr2_current);
  }

  //if scan complete print scan results
  if(scan_complete) {
        if(debug_scan_results) {
      	  Serial.println("--------------------------------------------");
          double rate = servo_angular_rate;
  	  Serial.print("Servo angular rate is: ");
  	  Serial.println(rate, 10);
          Serial.println("--------------------------------------------");
          for(int i=0; i!=number_of_scan_headings; i++) {
            Scan_point s = scan[i];
	    char buffer[50];
	    sprintf(buffer, "\t measurement at angle %d is: %d", s.heading, s.current_distance);
	    Serial.println(buffer);
          }
        }
	scan_complete=false;	//reset flag
  }
}

void ISR_increase_scaler() {
	scaler++;
}

const int* advance_order_pointer(byte b) {
  //takes the sensor_state as the argument
  const int* p = order_pointer;
  if (bitRead(b, 2)) {	//front sensor tripped..retest front sensor
    if(debug)
      Serial.println("font sensor tripped in get_next_scan_point");
    p = center;
  } else {
    p++;
    if (p == &scan_order[number_of_scan_stops]) {
      p = &scan_order[0];
    }
  }
  return p;
}

int find_scan_target(int target_index) {
  //argument is the dereferenced order_pointer integer
  //returns the index into scan[] for the correct Scan_point
  int res;
  for (int i=0; i!=number_of_scan_headings; i++) {
    if(scan[i].index == target_index) {
      res = i;
      //TODO: add exception for index not found
    }
  }
  return res;  
}

int get_heading(int p) {
  Scan_point s = scan[p];
  int heading = s.heading;
  return heading;
}

unsigned long find_ready_time(int h, unsigned long t, double rate) {
	int current_heading = scanner.read();
	int delta = current_heading - h;
	delta = abs(delta);
	double base_delay = delta * rate  * 1000;	//delay in milliseconds
	unsigned long ready_time = (long)base_delay + t;
        if(debug) {
          char buffer[75];
          sprintf(buffer, "\t delta is: %d\n\t current heading is: %d\n\t target heading is: %d", delta, current_heading, h);
          Serial.println(buffer);
          Serial.print("\t rate is: ");
          Serial.println(rate);
        }
	return ready_time;
}

bool this_is_last_scan_point(const int* p) {
  bool result = false;
  p++;
  if(p == &scan_order[number_of_scan_headings])
	result = true;
  return result;
}
/*----------Motor related functions-------------*/
int update_control(const byte b, const boolean _stall) {
	int controls_index = 0;
        //if stalled
        if(_stall) {
          controls_index = find_control_index("backward");
          return controls_index;
        }
        //if clear on all bits
        if(b == B00000) {    //no bits set
                controls_index = find_control_index("forward");
                return controls_index;
        }
	//front sensor tripped
	if (bitRead(b, 2)) {  //center sensor is tripped then rotate right
		controls_index = find_control_index("twist_right");
                return controls_index;
	}
	//forward quarter sensors tripped with middle clear
	if (bitRead(b, 3) && !bitRead(b, 2)) { //left-forward tripped but middle clear then swerve right
		controls_index = find_control_index("swerve_right");
                return controls_index;	
	}
	if (bitRead(b, 1) && !bitRead(b, 2)) {//rigt-forward tripped but middle clear then swerve left
		controls_index = find_control_index("swerve_left");
                return controls_index;
	}
	//just a trip on the side sensors then ease right or left
	switch(b) {
	case B10000:	//left side trip... ease to the right
		controls_index = find_control_index("ease_right");
                return controls_index;
	case B00001:	//right side trip...ease to the left by slowing that side down
		controls_index = find_control_index("ease_left");
                return controls_index;
        }
}
void drive(int i) {
  Control_vec v = controls[i];
  StateVector rt = {v.m1_dir, v.m1_pwm};
  StateVector lt = {v.m2_dir, v.m2_pwm};
  mtr_rt.updateState(rt);
  mtr_lt.updateState(lt);  
}
int find_control_index(const char* target) {
  int res;
  for (int i=0; i!=number_of_controls; i++) {
     const char* control_label = controls[i].label;
     if(!strcmp(control_label, target)) {
       res = i;
     }
  }
  return res;
}
