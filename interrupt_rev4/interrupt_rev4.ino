/* Increment a counter by using an external interrupt. 
   Include a maint switch that prevents the counter from incrementing while in maint mode.
   
   This implementation doesn't control interrupts on the microcontroller.  Instead it
   takes the raw button input on pin 4, debounces it via software and provides the 
   debounced output on pin 3.  Pin 3 is then jumpered into the interrupt pin (D2).  The
   maint switch input on pin 11 is held high by the pullup resistor.  When you flip 
   the maint switch and take pin 11 low, it stops sending data to pin 3 and therefore
   it prevents any further interrupts from processing.
   
   By Zac Staples, July 2013.
*/

//Includes
#include <Bounce.h>

//constants
const int int_pin =2;
const int debounced_button_result_pin = 3;
const int button_pin = 4;
const int maint_switch_pin = 11;
const int led = 13;
const byte print_count = 0x01;
const byte print_maint_status = 0x02;

//volatiles updated by the interrupt service routine
volatile byte state = HIGH;
volatile unsigned int count = 0;
volatile byte print_flags = 0x01;  //print_count true at config

//other global variables
Bounce button = Bounce(button_pin, 5);  //five millisecond debounce time
boolean in_maint_mode = false;

void setup() {
  Serial.begin(9600);
//  pinMode(int_pin, OUTPUT);  //set interrupt pin to OUTPUT so I can write to it
  attachInterrupt(0, blink, FALLING); //only trigger interrupt once per button push, as opposed to CHANGE which would have yielded two int events per push
  pinMode(debounced_button_result_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);  
  pinMode(maint_switch_pin, INPUT_PULLUP);
  pinMode(led,OUTPUT);
  count = 0;

}

void loop() {
  //test for maint mode and set global variables
  if (digitalRead(maint_switch_pin) == LOW) {
    in_maint_mode = true;
    print_flags |= print_maint_status;
  } else {
    in_maint_mode = false;
    clear_maint_flag();
  }
  
  //monitor the button only if not in maint mode
  if(!in_maint_mode) {
     button.update();
//     if(button.read() == HIGH) 
//       Serial.println("Button is high");
     digitalWrite(3, button.read());  //toggle the interrupt based on the debounced button
     digitalWrite(led, state);
  } 
  
  //show count and maint mode in the serial monitor
  print_if_required(print_flags);
}

//ISR
void blink() {
  state = !state;
  count++;
  print_flags |= print_count;
}

//Configure output
void print_if_required(byte flags) {
  if (flags & print_count) {
    char buffer[50];
    sprintf(buffer, "Count is now: %d", count);
    Serial.println(buffer);
    clear_print_flag();
  }
  if (flags & print_maint_status) {
    Serial.println("In maint mode");
  }
}

void clear_print_flag() {
  print_flags &= ~print_count;  //print_flags & B1111 1110
}

void clear_maint_flag() {
  print_flags &= ~print_maint_status;  //print_flags & B1111 1101  
}
