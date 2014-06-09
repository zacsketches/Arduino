#include <iostream>

using namespace std;

struct Scan_order{
    int* order;
    int pos;
    int sz;

    Scan_order(int test_points);   //test_points is the unique points in the scan
                                    //the test order will come back to the middle after every
                                    //other measurement like the examples below for a 7 and 5 point scan
                                    //where 0 is the middle
                                    //      0 1 0 2 0 3 0 4 0 5 0 6
                                    //      0 1 0 2 0 3 0 4
    ~Scan_order() { delete[] order; } 

    Scan_order& operator++() {    //prefix ++
        ++pos;
        if (pos == sz) pos = 0;
        return *this;
    }
    int current() {return order[pos]; }
};

//*******************************************************************
//*                         SCAN ORDER CONSTRUCTOR
//*******************************************************************
Scan_order::Scan_order(int test_points) {
    pos = 0;
    sz = 2* (test_points -1);
    order = new int[sz];
    int mult = 1;
    for(int i = 0; i < sz; ++i) {
        if(i%2 == 0) order[i] = 0;    //always come back to the middle
        else {
            order[i] = mult;
            ++mult;
        }
    }     
}

int main (int argc, char const *argv[])
{
    int test_points = 9;
    Scan_order scan_order(test_points);

    for(int i = 0; i < scan_order.sz; ++i) {
        cout<<"current is: "<<scan_order.current()<<endl;
        ++scan_order;
    }
    
    return 0;
}

