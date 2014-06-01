/* Increment a counter by using an external interrupt. */
/* rev1 - instead of writing to pin3 and then jumpering that into pin2, I want to
   try and write directely to pin 2.
*/

#include <Bounce.h>

volatile int state = HIGH;

const int int_pin =2;
const int button_pin = 4;
const int led = 13;

Bounce button = Bounce(button_pin, 5);  //five millisecond debounce time

void setup() {
  attachInterrupt(0, blink, FALLING);
  pinMode(int_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led,OUTPUT);
}

void loop() {
 button.update();
 //The next line of code is the one under test here.  We take the result of a 
 //debounced button and write the result to the interrupt monitoring pin.
 digitalWrite(int_pin, button.read());
 digitalWrite(led, state);
}

void blink() {
  state = !state;
}
