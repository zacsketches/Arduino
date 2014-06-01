/*  On 6/1/2014 I committed to getting my Alfred robot up and running again.
    This time I want to take what I've leared about C++ and build a few data
    structures that support an easy implementation for a servo that rotates and
    takes ultrasonic measurements.

    This is a non-arduino compatible c++ driver to test my library data structures.
*/

#include "Scanner.h"
#include <iostream>

using namespace std;

void test_scan_point() {
  cout<<"************************************************************\n";
  cout<<"TESTING SCAN_POINT\n";

  //default construct a scan_point
  Scan_point a(100);
  cout<<"a's heading is: "<<a.heading()<<" and a's data is: "<<a.data()<<endl;
  cout<<"setting heading\n";
  a.set_heading(90);
  cout<<"a's heading is: "<<a.heading()<<" and a's data is: "<<a.data()<<endl;
  
  
  //construct a scan point with two params provided
  Scan_point b(100, 50000);
  cout<<"b's heading is: "<<b.heading()<<" and b's data is: "<<b.data()<<endl;
  cout<<"setting data\n";
  b.set_data(100000);
  cout<<"b's heading is: "<<b.heading()<<" and b's data is: "<<b.data()<<endl;
}

int main (int argc, char const *argv[])
{
  test_scan_point();
  return 0;
}

