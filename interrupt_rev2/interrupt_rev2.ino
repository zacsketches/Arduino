/* Increment a counter by using an external interrupt. */
/* rev1 - instead of writing to pin3 and then jumpering that into pin2, I want to
   try and write directely to pin 2.
   rev2 - add a volatile counter and a flag to determine whether to reprint the counter to
   the serial monitor.
*/

#include <Bounce.h>

volatile byte state = HIGH;
volatile unsigned int count = 0;
volatile byte reprint = true;

const int int_pin =2;
const int button_pin = 4;
const int led = 13;

Bounce button = Bounce(button_pin, 5);  //five millisecond debounce time

void setup() {
  Serial.begin(9600);
  pinMode(int_pin, OUTPUT);
  attachInterrupt(0, blink, FALLING);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led,OUTPUT);
  count = 0;

}

void loop() {
 button.update();
 //The next line of code is the one under test here.  We take the result of a 
 //debounced button and write the result to the interrupt monitoring pin.
 digitalWrite(int_pin, button.read());
 digitalWrite(led, state);
 print_if_required(reprint);
}

void blink() {
  state = !state;
  count++;
  reprint = true;
}

void print_if_required(bool flag) {
  if (flag) {
    char buffer[50];
    sprintf(buffer, "Count is now: %d", count);
    Serial.println(buffer);
    clear_print_flag();
  }
}

void clear_print_flag() {
  reprint = false;
}
