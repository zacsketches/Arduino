/* 
  Rover5Motor.h - Library for controlling a single motor via the 
  Rover 5 motor controller by Dagu.
  Created by Zac Staples, 30 Jun 2013.
  All rights reserved.  Any unauthorized copy or reproduction of this
  code in part or in its entirety is a violation of intellectual 
  property rights held by its creator.
*/
#ifndef Rover5Motor2_h
#define Rover5Motor2_h

#include "Arduino.h"

struct StateVector {
 	int dir;
 	int pwm;
};

class Rover5Motor2{
private:
	int pin_pwm;
	int pin_dir;
	StateVector vec;
public:
	Rover5Motor2(int pinPWM, int pinDir);
	void stop();
	void drive(StateVector v);
	void updateState(StateVector& d);
	const StateVector getState();
};

#endif