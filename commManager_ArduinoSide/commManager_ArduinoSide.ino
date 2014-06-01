const int signalPIN = 13; //use the onboard led for this example

void setup() {
  // initialize serial communication:
  Serial.begin(9600); 
  establishContact();  // send a byte to establish contact until receiver responds 

  pinMode(signalPIN, OUTPUT); //set the signal pin ready for output
}

void loop() {
  //check the serial port for data
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    // do something different depending on the character received.  
    // The switch statement expects single number values for each case;
    // in this exmaple, though, you're using single quotes to tell
    // the controller to get the ASCII value for the character.  For 
    // example 'a' = 97, 'b' = 98, and so forth:

    switch (inByte) {
    case 'm':  //Processing is requesting a measurement    
      //blink the LED quickly
      for (int i=0; i<8; i++) {
        digitalWrite(signalPIN, HIGH);
        delay(200);
        digitalWrite(signalPIN, LOW);
        delay(200);
      }
      break;
    case 'p':  //Processing is requesting a position update
      //blink the LED slowly
      for (int i=0; i<8; i++) {
        digitalWrite(signalPIN, HIGH);
        delay(1000);
        digitalWrite(signalPIN, LOW);
        delay(1000);
      }
      break;
    } 
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}
