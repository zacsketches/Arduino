/*
  TestClass.h - library for learning how to write libraries
  in the Arduino IDE.  
  Zac Staples 5 May, 2013.
*/
#ifndef TestClass_h
#define TestClass_h

#include "Arduino.h"

class TestClass {
	int _i;
public:
	TestClass();
	void addOne();
	int getI();
	boolean typeTest;
private:
//	int _i;
};

#endif
