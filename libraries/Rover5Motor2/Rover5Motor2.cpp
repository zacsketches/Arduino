/* 
  Rover5Motor.h - Library for controlling a single motor via the 
  Rover 5 motor controller by Dagu.
  Created by Zac Staples, 30 Jun 2013.
  All rights reserved.  Any unauthorized copy or reproduction of this
  code in part or in its entirety is a violation of intellectual 
  property rights held by its creator.
*/

#include "Arduino.h"
#include "Rover5Motor2.h"			//get the interface

Rover5Motor2::Rover5Motor2(int pinPWM, int pinDir) {
	pinMode(pinPWM,OUTPUT);
	pinMode(pinDir,OUTPUT);
	pin_pwm = pinPWM;
	pin_dir = pinDir;
	vec.dir = 0;
	vec.pwm = 0; 
}

void Rover5Motor2::stop() {
	vec.pwm =0;
	vec.dir =0;
	updateState(vec);
}
void Rover5Motor2::drive(StateVector v){
	digitalWrite(pin_dir, v.dir);		
	analogWrite(pin_pwm, v.pwm);
}
void Rover5Motor2::updateState(StateVector& d){
	if(d.dir>=0 && d.dir<=1)		//rover5 dir limited to 1 or 0
		vec.dir = d.dir;
	if(d.pwm>=0 && d.pwm<=255)		//arduino limited to PWM output 0-255
		vec.pwm = d.pwm;
	drive(vec);
}
const StateVector Rover5Motor2::getState(){
	return vec;
}