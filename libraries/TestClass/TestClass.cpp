#include "Arduino.h"
#include "TestClass.h"

TestClass::TestClass() {
	_i = 0;
	typeTest = true;
}

void TestClass::addOne(){
	if(typeTest){
		_i+=1;
	}
}

int TestClass::getI(){
	int x = _i;
	return x;
}
