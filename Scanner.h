/*
    TODO add attribution info
*/

/* 
   Scanner implements control over an ultrsonic sensor mounted to a 
   servo in order to scan multiple headings with a single sensor.
   
   The signature for creating a scanner is:
   Scanner(const int servo_pin, 
           const int ping_pin, 
           const int center = 90,
           const int span = 180,
           const int test_points = 5,
           const int servo_angular_rate = 280/60+5);	in millisec / deg.  Futaba S3004 280us/60deg plus a tad
             
    Calling .data() or .readings() returns a JSON formatted string of the most recent
    data in the scanner.

    The .run() method will start the scanner.  Measurements in cm and data storage 
    are handled by the class with RAII principles.
*/

#ifndef Scanner_h
#define Scanner_h

#define DEBUG_SCAN 0        //1 for compilation outside of Arduino IDE
#define DEBUG_SER  1        //1 for debug prints to adruino Serial.print cmd

#if DEBUG_SCAN == 0
    #include <Arduino.h>
#endif

#if DEBUG_SCAN == 1
    #include <iostream>
#endif

#include <Servo.h>

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
  const int heading_by_index(const int index) const;
  
  const char* data() const;            //JSON format pointer to dat[]
  
  // MODIFYING METHODS //            
  bool update_by_heading(const int heading, const int data);  //returns true on successful update
  bool update_by_index(const int index, const int data);
  
};

//*******************************************************************
//*                         SCANNER CLASS
//*******************************************************************

class Scanner : public Servo {
private: 
  //Servo data
  int sp;                 //Servo pin
  int ctr;                //where the servo points directly ahead of the bot
  Servo servo;            //Servo object
  int sar;                //servo_angular_rate the servo turns out
  char servo_state;       // B0001->servo ready 
  					      // B0010->move ordered 
                          // B0100->move complete
  struct Scan_order{
      int* order;
      int pos;
      int sz;

      Scan_order(int test_points);   //test_points is the unique points in the scan
                                      //the test order will come back to the middle after every
                                      //other measurement like the examples below for a 7 and 5 point scan
                                      //where 0 is the middle
                                      //      0 1 0 2 0 3 0 4 0 5 0 6
                                      //      0 1 0 2 0 3 0 4
      ~Scan_order() { delete[] order; } 

      Scan_order& operator++() {    //prefix ++
          ++pos;
          if (pos == sz) pos = 0;
          return *this;
      }
      int current() {return order[pos]; }
  };
 

  //Ultrasonic sensor data
  int pp;               //ping_pin

  //Scan object handles data for the scanner
  Scan  scan;
  Scan_order scan_order;
  
  int find_delay(int target_angle);
  void take_reading();       
  long pulse();
  int us_to_cm(const long duration); 

public:
  //constructor
  /*
    TODO store servo_angular rate in eeprom.  
    Then try to read it before setting default
  */
  Scanner(const int servo_pin, 
          const int ping_pin, 
          const int center = 90,
          const int span = 180,
          const int test_points = 5,
          const int servo_angular_rate = 280/60+500);	//in millisec / deg.  Futaba S3004 280us/60deg plus a tad
          
  // NON-MODIFYING METHODS
  const char* headings() { return scan.headings(); }
  const char* data() { return scan.data(); }
  
  // MODIFYING METHODS
  void run();      //run a scan of all headings in the scan pattern
  
};  

#endif
