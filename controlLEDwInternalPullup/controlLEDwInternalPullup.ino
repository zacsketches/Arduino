//written by Dad on 2/18 to better understand how to write
//serial communication between the Arduino and Processing
//
//Button should be connected between ground and pin 2.  Depressing
//the button will drive pin 2 low from its internal pullup.

const int ledPin=13;
const int buttonPin=2;

int state = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin, HIGH); //set the LED to start on
}

void loop() {
 delay(10);  //debounce the switch
 int buttonValue = digitalRead(buttonPin);
 if(state != buttonValue) {
   state=buttonValue;
   digitalWrite(ledPin, state);
   Serial.println(state);  //write the value to the serial monitor 
 } 
}
