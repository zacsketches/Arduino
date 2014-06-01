/* Test my Scanner class to determine the best algorithm for delay between
   servo test points.

   The functional steps I want in this test are:
   1. Create a Scanner object
   2. Display config data for the scanner via Serial comms
   3. Make a scan and store results
   4. When a button is pushed decrease the angular rate and rescan.
*/
#include <Scanner.h>
#include <Bounce.h>

/*----Data structures--------*/
struct Scan_point{
	int index;		//1 to number_of_scan_headings
	int heading;		//in degrees
	long trip_distance;	//distance the sensor trips
	byte mask_value;	//1, 2, 4, 8, 16,	
	long current_distance;  //most recent reading at this point
};

/*------Constants------------*/
const int number_of_scan_headings = 5;    
const int scan_order[number_of_scan_headings] = {3, 2, 4, 1, 5};   //compared to Scan_point.index
const int servo_pin = 9;
const int ping_pin = 8;
const double base_angular_rate = .28/60;	//Futaba S3004 .28sec/60deg

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
byte servo_state = 0x00;			//B0001->servo ready 
					//B0010->move ordered 
					//B0100->move complete
					//B1000->sensor_state updated

/*-------Flow control flags---------*/
boolean scan_complete = false;

/*------Configuration Options--------*/
const bool debug = false;

void setup() {
  Serial.begin(9600);

  attachInterrupt(0, ISR_increase_scaler, FALLING);	//INT0 is on I/O 2 for arduino Uno
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(int_pin, OUTPUT);

  //configure scan points
  Scan_point p1 = {1, 170, 30, B10000};
  Scan_point p2 = {2, 130, 60, B01000};
  Scan_point p3 = {3, 90, 60,  B00100};  //center 
  Scan_point p4 = {4, 50, 60,  B00010};
  Scan_point p5 = {5, 10, 30,  B00001};

  //push the scan points into the scan array
  scan[0] = p1;
  scan[1] = p2;
  scan[2] = p3;
  scan[3] = p4;
  scan[4] = p5;

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
  static long cm;
  
  //updating the debounced button and writing the output to the interrupt pin
  button.update();
  digitalWrite(int_pin, button.read());

  servo_angular_rate = base_angular_rate + (scaler*multiple);

  //if servo is ready then order next scan
  if(servo_state & B0001 == 0x01) {	//B0001 tests servo_ready bit
	order_pointer = advance_order_pointer();	
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

  //if move ordered and time has elapsed for move then pulse
  if((servo_state == B0010) && ( millis()>=ready_time) ) {
	long duration = scanner.pulse(ping_pin);
	cm = scanner.microsecondsToCentimeters(duration);
	servo_state = B0100; 	//B0100->move_complete
  }

  //if move complete, compare measurement against trip distance and update sensor_state	
  if(servo_state == B0100) {	//B0100->move_complete
  	int scan_index = find_scan_target(*order_pointer);
	Scan_point s = scan[scan_index];
	if(cm <= s.trip_distance) {
		sensor_state |= s.mask_value;	//set the bit
	} else {
		sensor_state &= ~s.mask_value;	//clear the bit
	}
	servo_state = B1000;	//B1000->sensor_state updated
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

  //if scan complete print scan results
  if(scan_complete) {
	//show scanner donfig data
  	Serial.println("--------------------------------------------");
	double rate = servo_angular_rate;
  	Serial.print("Servo angular rate is: ");
  	Serial.println(rate, 10);
	//show scan results
	for(int i=0; i!=number_of_scan_headings; i++) {
	    Scan_point s = scan[i];
	    char buffer[50];
	    sprintf(buffer, "\t measurement at angle %d is: %d", s.heading, s.current_distance);
	    Serial.println(buffer);	}
	scan_complete=false;	//reset flag
  }
}

void ISR_increase_scaler() {
	scaler++;
}

const int* advance_order_pointer() {
  const int* p = order_pointer;
  p++;
  if(p == &scan_order[number_of_scan_headings]) {
    p = &scan_order[0];
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
