/* My test to understand how to use an interrupt service routine.
 * This circuit requires a multivibrator input to the interrupt pin.
 * I built my multivibrator on Greyson's electronics playground board
 * similar to the one in project #41
 */
 
 /*this is my first attempt at using my new coding tool on the iPad
   this program reads a multivibrator input on int0 and 
   blinks led13 at each change. 
  */
  volatile int state = LOW;
  int ledPin = 13;

  void setup() {
  	pinMode(ledPin, OUTPUT);
  	attachInterrupt(0, blinky, CHANGE);
  }

  void loop() {
  	digitalWrite(ledPin, state);
  }

  void blinky() {
  	state = !state;
  }
