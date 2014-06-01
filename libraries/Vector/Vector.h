/* 
  Vector.h - Library for implementing a Vector class
  with basic functionality resident in the C++11 vector.
  I want to use my Vector to send control signals to the motor
  controller for the Rover5.
  Created by Zac Staples, 30 Jun 2013.
  All rights reserved.  Any unauthorized copy or reproduction of this
  code in part or in its entirety is a violation of intellectual 
  property rights held by its creator.
*/

#ifndef Vector_h
#define Vector_h

#include "Arduino.h"

class Vector{
private:
	float* elem;			//elem points to an arry of sz doubles
	int sz;
public:
	Vector(int s);			//constructor
	~Vector();				//destructor	
	float& operator[] (int i);
	int size();
};

#endif