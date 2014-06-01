/*
 * Send binary sketh
 * Sends a header followed by two random integer values as binary data.
*/

const char HEADER = 'H';

void setup()
{
    Serial.begin(9600);
}

void loop() {
   Serial.print(HEADER);  //send a header
 
   //send a random integer
   //int test=random(230,325);
  
   int test=369;
  
  sendBinary(test);
 
  delay(2000);
 
  
}

void sendBinary(int value) {
 Serial.write(highByte(value));
 Serial.write(lowByte(value)); 
}
