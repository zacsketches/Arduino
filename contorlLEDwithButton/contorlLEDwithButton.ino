//written by Dad on 2/18 to better understand how to write
//serial communication between the Arduino and Processing

const int ledPin=13;
const int buttonPin=2;
int buttonUp=HIGH;
int buttonDown=LOW;
int led = HIGH;

int state = 1;
int lastValue=buttonUp;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
 delay(10);  //debounce the switch
 int buttonValue = digitalRead(buttonPin);
 if(buttonValue==buttonUp && lastValue==buttonDown) {
   state=state*-1;
   if (state == -1) {
      led = LOW; 
   } else {
      led = HIGH;
   }
  digitalWrite(ledPin, led);
  Serial.println(state);  //write the value to the serial monitor 
 }
 lastValue=buttonValue;    
}
