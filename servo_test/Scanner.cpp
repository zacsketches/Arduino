#include "Arduino.h"
#include "Scanner.h"


Scanner::Scanner(){
	//default const
}

void Scanner::attach(int servoPin, int pingPin){
	servo_pin = servoPin;
	ping_pin = pingPin;
	
	span=160;						//degrees
	test_points=5;					//number of location the scanner stops to take a meaurement
	span_center=90;					//degrees
	scan_order[0] = 2;
	scan_order[1] = 1;
	scan_order[2] = 3;
	scan_order[3] = 0;
	scan_order[4] = 4;
	// scan_order = {2, 1, 3, 0, 4};	//values refer to index of scan_points
	scan_delay=500;				//milliseconds
	
	jack_servo.attach(servo_pin);
		
	configure_scan();
}

int Scanner::get_attach_pin(){
	return servo_pin;
}

int Scanner::get_test_point(int p) {
	return scan_points[p];
}

int Scanner::read_servo(){
	return jack_servo.read();
}

void Scanner::configure_scan(){
	//compute the scan_points and fill the array
	int segments = test_points-1;
	int segment_width = round(span/segments);
	
	int start_point = span_center - (.5 * segments * segment_width);	//first point
	
	int* p = scan_points;
	
	for (int i=0; i!=test_points; i++){			//fill scan_points array
		*p = start_point + segment_width * i;
		p++;
	}
}

void Scanner::write(int d){
	jack_servo.write(d);
}

void Scanner::scan() {
	//rotate the scan servo through the scan pattern
	for(int i=0; i!=test_points; i++){
		int j = scan_order[i];
		jack_servo.write(scan_points[j]);
		delay(scan_delay);
	}
}
