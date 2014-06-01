#include <Scanner.h>
#include <Rover5Motor2.h>

/*----Data structures--------*/
struct Scan_point{
	int index;		//1 to number_of_scan_headings
	int heading;		//in degrees
	long trip_distance;	//distance the sensor trips
	byte mask_value;	//1, 2, 4, 8, 16, 32, 64, or 128	
};
struct Control_vec{
	char label[13];
        int m1_dir;
	int m1_pwm;
	int m2_dir;
	int m2_pwm;
	bool overwrite;		//true if the control_vec is to completely overwrite the existing vector.
};

/*--------Constants---------*/
const Control_vec forward = {"forward", 0, 80, 0, 80, true};
const Control_vec backward = {"backward", 0, 100, 1, 100, true};
const Control_vec ease_right = {"ease_right", 0, 150, 0, 100, true};
const Control_vec ease_left = {"ease_left", 0, 100, 0, 150, true};
const Control_vec swerve_right = {"swerve_right", 0, 80, 0, 200, true};
const Control_vec swerve_left = {"swerve_left", 0, 200, 0, 80, true};
const Control_vec twist_right = {"twist_right", 1, 150, 0, 150, true};
const int number_of_controls = 7;
const int number_of_scan_headings = 5;    
const int scan_order[number_of_scan_headings] = {3, 2, 4, 1, 5};   //index numbers of scan[]

/*--------Global Variables----------*/
byte sensor_bits = B00000;		//bit mask represents scan results.  0 represents a clear scan
Scan_point scan[number_of_scan_headings];
Control_vec controls[number_of_controls];  //array initialized in setup
const int* order_pointer = scan_order;
Scanner scanner;                           //requires attach function in setup  
Rover5Motor2 mtr_rt = Rover5Motor2(5,4);
Rover5Motor2 mtr_lt = Rover5Motor2(6,7);

/*------Configuration Options--------*/
const bool debug = false;
const bool motor_debug = true;

void setup() {
  Serial.begin(9600);
  
  //configure scan points
  Scan_point p1 = {1, 170, 30, B10000};
  Scan_point p2 = {2, 130, 60, B01000};
  Scan_point p3 = {3, 90, 60,  B00100};  //center 
  Scan_point p4 = {4, 50, 60,  B00010};
  Scan_point p5 = {5, 10, 30,  B00001};
  
  //push the scan points into the array
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
  scanner.attach(9, 8);  //jack servo on pin 9 and PING))) on pin 8
    
}

void loop() {
  order_pointer = get_next_scan_point(sensor_bits);
  int scan_index = find_scan_target(*order_pointer);
  bool b = test_point(scan_index);
  sensor_bits = update_sensor_bits(scan_index, b);
  if(debug) {
    Serial.print("The sensor_bits are: ");
    Serial.println(sensor_bits, BIN);
  }
  int control_index = update_control(sensor_bits);
  if(motor_debug) {
    char* debug_label = controls[control_index].label;
    Serial.print("The control lable is: ");
    Serial.println(debug_label);
  }
  drive(control_index);
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

int update_control(byte b) {
	int controls_index = 0;

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

byte update_sensor_bits (int target_index, bool b) {
   byte res = sensor_bits;
   Scan_point s = scan[target_index];
  if(b) {
    res |= s.mask_value;
  } else {
    byte tmp_byte = ~s.mask_value;
    res &= tmp_byte;
  }
  return res;
}

bool test_point(int p) {
  bool res = false;
  Scan_point s = scan[p];
  int heading = s.heading;
  long cm = scanner.measure_angle(heading);
  if(debug) {
    Serial.print("The measured distance at index ");
    Serial.print(s.index);
    Serial.print(" is: ");
    Serial.println(cm);
  }
  if (cm <= s.trip_distance) {
    res = true;
    if(debug) 
      Serial.println("The sensor is tripped");
  }
  else {
    res = false;
  }
  return res; 
}

const int* get_next_scan_point(byte b) {
  const int* p = scan_order;
  if (bitRead(b, 2)) {	//front sensor tripped..retest front sensor
    if(debug)
      Serial.println("font sensor tripped in get_next_scan_point");
    p = &scan_order[0];
  } else {
    p = order_pointer;
    p++;
    if (p == &scan_order[5]) {
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
    }
  }
  return res;  
}
