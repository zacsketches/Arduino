//wrote to test the setup for 2D arrays in Arduino
//This little program shows that you can create a two-dimensional array
//Then you can address each zero based index.  [0][0] is the top left of the array

void setup() {
  // open the serial port at 9600 bps:
  Serial.begin(9600);  
}

void loop(){
  delay(3000);
  int val;
    int my2DArray[2][3]={
    {1,2,3},
    {4,5,6}
  };
  
  val=my2DArray[0][0];
  Serial.println(val);
  val=my2DArray[0][1];
  Serial.println(val);
  val=my2DArray[1][1];
  Serial.println(val);
}
