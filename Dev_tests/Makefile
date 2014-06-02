# basic build for my dev_test
CC= g++
CFLAGS=-c -I/Applications/Arduino.app/Contents/Resources/Java/libraries/Servo -I/Users/zhs/Documents/Arduino/libraries/Scanner
LFLAGS= -L/Applications/Arduino.app/Contents/Resources/Java/libraries/Servo

all: a_scan_test
	
a_scan_test: data_struc.o Scanner.o Servo.o
	$(CC) $(LFLAGS) data_struc.o Scanner.o Servo.o -o a_scan_test
	
data_struc.o: data_struc.cpp  
	$(CC) $(CFLAGS) data_struc.cpp
	
Scanner.o: /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.cpp /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.h
	$(CC) $(CFLAGS) /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.cpp
	
Servo.o: Servo.cpp
    $(CC) $(CFLAGS) Servo.cpp
    

clean:
	rm -rf *.o a_scan_test
