/* Increment a counter using an external interrupt. 
   Include a maint switch that prevents the counter from incrementing while in maint mode.
   
   This implementation doesn't control interrupts on the microcontroller.  Instead it
   takes the data input (encoder simulated by a button in this case) and routes 
   it through a data enable switch.  This data enable switch cuts the data in/out
   and prevents further interrups.
   
   Count is displayed on the serial monitor and can be observed to to hold steady
   despite additional "pushes" while data is not enabled.
   
   By Zac Staples, July 2013.
*/

//constants
const int int_pin =2;
const int led = 13;
const byte print_count = 0x01;

//volatiles updated by the interrupt service routine
volatile byte state = HIGH;
volatile unsigned int count = 0;
volatile byte print_flags = 0x01;  //print_count true at config

void setup() {
  Serial.begin(9600);
  pinMode(int_pin, INPUT_PULLUP); 
  attachInterrupt(0, blink, FALLING); 
  pinMode(led,OUTPUT);
  count = 0;

}

void loop() {
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
