/* Increment a counter by using an external interrupt. */

#include <Bounce.h>

volatile int state = LOW;

const int int_pin =2;
const int debounced_button_pin = 3;
const int button_pin = 4;
const int led = 13;

Bounce button = Bounce(button_pin, 5);  //five millisecond debounce time

void setup() {
  attachInterrupt(0, blink, CHANGE);
  pinMode(debounced_button_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led,OUTPUT);
}

void loop() {
 button.update();
 digitalWrite(debounced_button_pin, button.read());  //read the debounced result and set the result pin accordingly
 digitalWrite(led, state);
}

void blink() {
  state = !state;
}
