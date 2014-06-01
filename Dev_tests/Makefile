# basic build for my dev_test
CC= g++
CFLAGS=-c -I/Applications/Arduino.app/Contents/Resources/Java/libraries/Servo
LFLAGS=-L/Applications/Arduino.app/Contents/Resources/Java/libraries/Servo

all: a_scan_test
	
a_scan_test: data_struc.o Scanner.o
	$(CC) $(LFLAGS) data_struc.o Scanner.o -o a_scan_test
	
data_struc.o: data_struc.cpp  
	$(CC) $(CFLAGS) data_struc.cpp

Scanner.o: Scanner.cpp Scanner.h
	$(CC) $(CFLAGS) Scanner.cpp

clean:
	rm -rf *.o a_scan_test
