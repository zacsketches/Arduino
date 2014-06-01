/*
  Dad and Hayden did this as our second project to learn about the PWM ports
  */

// Dim an LED using a PWM pin
int PWMpin = 10; // LED in series with 470 ohm resistor on pin 10

void setup()
{
  // no setup needed
}

void loop()
/*{
   for (int i=0; i <= 255; i++){
      analogWrite(PWMpin, i);
      delay(20);
   } 
}
*/
{
  analogWrite(PWMpin, 200);
}

