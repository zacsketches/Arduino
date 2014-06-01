#include <Scanner.h>

Scanner scanner;

struct Control_vec {
	int m1_dir;
	int m1_pwm;
	int m2_dir;
	int m2_pwm;
	bool overwrite;		//true if the control_vec is to completely overwrite the existing vector.
};

const Control_vec forward = {0, 150, 0, 150, true};
const Control_vec backward = {1, 100, 1, 100, true};
const Control_vec ease_right = {0, 150, 0, 100, true};
const Control_vec ease_left = {0, 100, 0, 150, true};
const Control_vec swerve_right = {0, 200, 0, 100, true};
const Control_vec swerve_left = {0, 100, 0, 200, true};
const Control_vec twist_right = {0, 100, 1, 100, true};

struct Scan_point {
	int index;		//1 to number_of_scan_headings
	int heading;		//in degrees
	long trip_distance;	//distance the sensor trips
	byte mask_value;	//1, 2, 4, 8, 16, 32, 64, or 128	
};

const int number_of_scan_headings = 5;	//user defined between 1-8.  Limitation of 8 is based on a 8 bit mask
					//for the sensor states.

Scan_point scan[number_of_scan_headings];		//array of length number_of_scan_headings

/* array of length number_of_scan_headings with values representing
the index from scan[] in the order user wants to 
scan. For example {2, 1, 3, 0} scans scan[2] then 
scan[1] etc..
*/
const int scan_order[number_of_scan_headings] = {3, 4, 2, 5, 1}; 
const int* order_pointer = scan_order;

byte sensor_bits = B00000;		//bit mask represents scan results.  0 represents a clear scan

void setup() {
  Serial.begin(9600);
  
  //configure scan points
  Scan_point p1 = {1, 170, 30, B00001};
  Scan_point p2 = {2, 130, 30, B00010};
  Scan_point p3 = {3, 90, 30,  B00100};
  Scan_point p4 = {4, 50, 30,  B01000};
  Scan_point p5 = {5, 10, 30,  B10000};
  
  //push the scan points into the array
  scan[0] = p1;
  scan[1] = p2;
  scan[2] = p3;
  scan[3] = p4;
  scan[4] = p5;
    
}

void loop() {
  advance_order_pointer();
  Serial.print(*order_pointer);
  int target = find_scan_target(*order_pointer);
  long cm = scanner.measure_angle(scan[target].heading);
  
  
  

  
  
  delay(500);
}

void advance_order_pointer() {
  order_pointer++;
  if (order_pointer == &scan_order[5]) {
    order_pointer = scan_order;
  }
}

int find_scan_target(int index2) {
  //argument is a scan index number
  //returns the index number of the scan[] up next
  int res = 0;
  for (int i=0; i!=number_of_scan_headings; i++) {
    if(scan[i].index == index2) {
      res = i;
    }
  }
  return res;  
}
