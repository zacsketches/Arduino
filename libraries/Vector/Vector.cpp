/* 
  Vector.cpp - Library for implementing a Vector class
  with basic functionality resident in the C++11 vector.
  I want to use my Vector to send control signals to the motor
  controller for the Rover5.
  Created by Zac Staples, 30 Jun 2013.
  All rights reserved.  Any unauthorized copy or reproduction of this
  code in part or in its entirety is a violation of intellectual 
  property rights held by its creator.
*/

#include "Arduino.h"
#include "Vector.h"			//get the interface

Vector::Vector(int s) {
	elem[s];
	sz = s;
}
Vector::~Vector() {free(elem);}
		
float& Vector::operator[] (int i){
	return elem[i];
}

int Vector::size() {
	return sz;
}