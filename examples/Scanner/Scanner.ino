/* 
   Scanner implements control over an ultrsonic sensor mounted to a 
   servo in order to scan multiple headings with a single sensor.
   
   The signature for creating a scanner is:
   Scanner(const int servo_pin, 
             const int ping_pin, 
             const int center = 90,
             const int span = 180,
             const int test_points = 5);
*/

#include <Scanner.h>

/*--------Global Variables----------*/
Scanner scanner;                           

/*------Configuration Options--------*/
const bool debug = false;

void setup() {
  Serial.begin(9600);

  
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
