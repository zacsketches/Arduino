Arduino Scanner
===============

Scanner implements control over an ultrsonic sensor mounted to a  servo in order to scan multiple headings with a single sensor.
      
The signature for creating a scanner is:

```c++
   Scanner(const int servo_pin, 
           const int ping_pin, 
           const int center = 90,
           const int span = 170,
           const int test_points = 5,
           const int servo_angular_rate = 240/60+5);	//in millisec / deg.  Futaba S3004 280ms/60deg
```

Calling .data() or .readings() returns a JSON formatted string of the most recent data in the scanner.

You must run .attach() in the setup method of the Arduino sketch.

The .run() method will start the scanner.  
    
Measurements are in cm.
    
Data storage is handled by the class with RAII principles.  No need for users to call delete.
