/* This is a basic control scheme for the DAGU 5 Rover chassis.
 * There are functions for straight, turnRight, and turnLeft.
 *
 * Next improvments would be use the encoders to control distance and turn angle
 */

int rotaryCount = 0;
byte phase;
unsigned long start;
int time_to_go;

#define PINA 8
#define PINB 9
#define INTERRUPT 0  // that is, pin 2

#define DIRECTIONA 4
#define MOTORA 5

#define DIRECTIONB 7
#define MOTORB 6

#define TIME_FORWARDS 10000
#define TIME_BACKWARDS 10000
#define TIME_TURN 1200

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int led = 13;
const int blinkDelay = 750;

void setup ()
{
//  attachInterrupt (INTERRUPT, isr, CHANGE);   // interrupt 0 is pin 2, interrupt 1 is pin 3
  pinMode (MOTORA, OUTPUT);
  pinMode (DIRECTIONA, OUTPUT);
  pinMode (MOTORB, OUTPUT);
  pinMode (DIRECTIONB, OUTPUT);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  delay(2000);
  blinkLed(4);
     
  //A
  straight(3000);
  delay(1000);
  //1
  turnRight(2100);
  //B
  straight(3000);
  //2
  turnLeft(2100);
  //C
  straight(2900);
  //3
  turnLeft(1400);
  //D
  straight(1100);
  //4
  turnLeft(1000);
  //E
  straight(5000);
  //5
  turnLeft(2100);
  //f
  straight(6000);
  //6
  turnRight(10000);
}  // end of setup

void loop ()
{
  delay(1000);
}

void straight(int howLong) {
  long start = millis();
  while (millis() < (start + howLong)) { 
    digitalWrite (DIRECTIONA, 0); 
    digitalWrite (DIRECTIONB, 0); 
    analogWrite (MOTORA, 200);
    analogWrite (MOTORB, 200);
  }
  analogWrite (MOTORA, 0);
  analogWrite (MOTORB, 0);
}

void turnRight(int howLong) {
 long start = millis();
  while (millis() < (start + howLong)) { 
    digitalWrite (DIRECTIONA, 1); 
    digitalWrite (DIRECTIONB, 0); 
    analogWrite (MOTORA, 150);
    analogWrite (MOTORB, 150);
  }
  analogWrite (MOTORA, 0);
  analogWrite (MOTORB, 0);
}

void turnLeft(int howLong) {
 long start = millis();
  while (millis() < (start + howLong)) { 
    digitalWrite (DIRECTIONA, 0); 
    digitalWrite (DIRECTIONB, 1); 
    analogWrite (MOTORA, 150);
    analogWrite (MOTORB, 150);
  }
  analogWrite (MOTORA, 0);
  analogWrite (MOTORB, 0);
}

void blinkLed(int blinks){
  for(int i = 0; i < blinks; i++) {
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(blinkDelay);               // wait for a second
      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
      delay(blinkDelay);               // wait for a second
  }
}
