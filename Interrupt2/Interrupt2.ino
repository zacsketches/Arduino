/* My test to understand how to use an interrupt service routine.
 * This circuit takes the interrupt signal from a motor encoder and
 * blinks the led for a second every 1000 counts.
 */
 
 /* The configuration for pinout in this test is:
  * 
  * Arduino Uno
    D2 - Int Output for Motor A
    D3 - Int Output for Motor B
    D4 - Direction Motor A
    D5 - PWM Motor A
    D6 - PWM Motor B
    D7 - Direction Motor B
  */  
 
  volatile int state = LOW;
  volatile unsigned int rotaryCount = 0;
  const int ledPin = 13;
  const int blinkDelay = 750;
  
  #define DIRECTIONA 4
  #define MOTORA 5
  
  #define DIRECTIONB 7
  #define MOTORB 6

  void setup() {
  	pinMode(ledPin, OUTPUT);
  	attachInterrupt(0, blinky, CHANGE);
        pinMode (MOTORA, OUTPUT);
        pinMode (DIRECTIONA, OUTPUT);
        pinMode (MOTORB, OUTPUT);
        pinMode (DIRECTIONB, OUTPUT);
        delay(3000);
        blinkLed(3);
  }

  void loop() {
  	digitalWrite(ledPin, state);
        digitalWrite (DIRECTIONA, 0); 
        analogWrite (MOTORA, 200);
  }

  void blinky() {
        rotaryCount++;
        if(rotaryCount % 1000 == 0) {
        	state = !state;
        }
  }
  void blinkLed(int blinks){
    for(int i = 0; i < blinks; i++) {
        digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(blinkDelay);               // wait for a second
        digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
        delay(blinkDelay);               // wait for a second
    }
  }
