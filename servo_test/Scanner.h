#include "servo.h"

class Scanner{
public:
	Scanner();
	void scan();
	int get_attach_pin();
	int get_test_point(int p);
	void write(int d);
	void attach(int servoPin, int pingPin);
	int read_servo();
	
private:
	int servo_pin;
	int ping_pin;
	int span;					//degrees
	int test_points;			//number of location the scanner stops to take a meaurement
	int span_center;			//degrees
	int scan_points[5];			//array of angles measurements are taken at
	int scan_order[];			//values refer to index of scan_points
	int scan_delay;				//milliseconds
	
	Servo jack_servo;
	
	void configure_scan();
};
