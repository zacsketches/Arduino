# basic build for my dev_test
CC= g++
CFLAGS=-c -I/Applications/Arduino.app/Contents/Resources/Java/libraries/Servo -I/Users/zhs/Documents/Arduino/libraries/Scanner

all: a_scan_test
	
a_scan_test: data_struc.o Scanner.o
	$(CC) data_struc.o Scanner.o -o a_scan_test
	
data_struc.o: data_struc.cpp  
	$(CC) $(CFLAGS) data_struc.cpp
	
Scanner.o: /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.cpp /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.h
	$(CC) $(CFLAGS) /Users/zhs/Documents/Arduino/libraries/Scanner/Scanner.cpp
	
    
clean:
	rm -rf *.o a_scan_test
