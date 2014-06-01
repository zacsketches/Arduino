/* Test my Scanner class to determine the best algorithm for delay between
   servo test points.
   
   The functional steps I want in this test are:
   1. Create a Scanner object
   2. Display config data for the scanner via Serial comms
   3. Make a scan and store results
   4. When a button is pushed decrease the angular rate and rescan.
   
*/

#include <Scanner.h>

/*----Data structures--------*/
struct Scan_point{
	int index;		//1 to number_of_scan_headings
	int heading;		//in degrees
	long trip_distance;	//distance the sensor trips
	byte mask_value;	//1, 2, 4, 8, 16, 32, 64, or 128	
};

/*------Constants------------*/
const int number_of_scan_headings = 5;    
const int scan_order[number_of_scan_headings] = {3, 2, 4, 1, 5};   //index numbers of scan[]

/*--------Global Variables----------*/
Scan_point scan[number_of_scan_headings];
const int* order_pointer = scan_order;
Scanner scanner;                           //requires attach function in setup  

/*------Configuration Options--------*/
const bool debug = false;

void setup() {
  Serial.begin(9600);
  
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
  scanner.attach(9, 8);  //jack servo on pin 9 and PING))) on pin 8
}

void loop() {
  //show scanner donfig data
  double rate = scanner.get_servo_angular_rate();
  char buffer[75];
  sprintf(buffer, "Scanner attached on pins 8 and 9.\nServo angular rate is: %f", rate);
  Serial.println(buffer);
  
  //test each scan point
  long scan_results[number_of_scan_headings];
  for(int i=0; i!=number_of_scan_headings; i++) {
    order_pointer = advance_order_pointer();
    int scan_index = find_scan_target(*order_pointer);
    scan_results[i] = test_point(scan_index);
  }
  
  //print scan results
  for(int i=0; i!=number_of_scan_headings; i++) {
    sprintf(buffer, "\t measurement at point %d is: %d", i, scan_results[i]);
    Serial.println(buffer);
  }
}

const int* advance_order_pointer() {
  const int* p = order_pointer;
  p++;
  if(p == &scan_order[5]) {
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
    }
  }
  return res;  
}

long test_point(int p) {
  Scan_point s = scan[p];
  int heading = s.heading;
  long cm = scanner.measure_angle(heading);
  if(debug) {
    Serial.print("The measured distance at index ");
    Serial.print(s.index);
    Serial.print(" is: ");
    Serial.println(cm);
  }

  return cm;
}
