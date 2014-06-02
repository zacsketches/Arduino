/*  On 6/1/2014 I committed to getting my Alfred robot up and running again.
    This time I want to take what I've leared about C++ and build a few data
    structures that support an easy implementation for a servo that rotates and
    takes ultrasonic measurements.

    This is a non-arduino compatible c++ driver to test my library data structures.
*/

#include "Scanner.h"
#include <iostream>

using namespace std;

//*******************************************************************
//*                         TEST SCAN_POINT
//*******************************************************************
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

//*******************************************************************
//*                         TEST SCAN
//*******************************************************************
void test_scan() {
    cout<<"************************************************************\n";
    cout<<"TESTING SCAN_POINT\n";
    
    //default construct a scan
    Scan s1;
    
    
    //controlled construction for all acceptable args
    Scan s2(7, 170, 93);
    cout<<"size of s2 is: "<<s2.size()<<" and span of s2 is: "<<s2.span()<<endl;
    
    //test data outputs
    const char* heads = s2.headings();
    cout<<"The JSON headings string is: "<<heads<<endl;
    const char* data = s2.data();
    cout<<"The JSON data string is: "<<data<<endl;
    
    //update data
    cout<<"updating data\n";
    s2.update_by_heading(93, 30000);
    s2.update_by_heading(121, 24320);
    s2.update_by_heading(65, 24320);
    s2.update_by_index(3, 12424);
    s2.update_by_index(5, 23324);
    s2.update_by_index(4, 31400);
    s2.update_by_index(6, 24000);
    
    heads = s2.headings();
    cout<<"The JSON headings string is: "<<heads<<endl;
    data = s2.data();
    cout<<"The JSON data string is: "<<data<<endl;
    
    //try invalid headings and indices
    if(!s2.update_by_heading(90, 999)) cout<<"bad heading returned false\n";
    if(!s2.update_by_index(-1, 999)) cout<<"bad index returned false\n";
    if(!s2.update_by_index(8, 999)) cout<<"bad index returned false\n";
    
    
    
  
}


//*******************************************************************
//*                         MAIN
//*******************************************************************
int main (int argc, char const *argv[])
{
  test_scan_point();
  test_scan();
  return 0;
}

