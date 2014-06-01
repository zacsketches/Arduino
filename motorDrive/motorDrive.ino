/*
  Dad and Hayden did this as our second project to learn about the PWM ports
  */

// Dim an LED using a PWM pin
int PWMpin = 10; // LED in series with 470 ohm resistor on pin 10
int motorPin = 5;

void setup()
{
  pinMode(motorPin, OUTPUT);
}

void loop()
{
/*   for (int i=80; i <= 255; i++)
   {
      analogWrite(PWMpin, i);
      delay(50);
   }
  analogWrite(PWMpin, 255);
  delay(3000);
  analogWrite(PWMpin, 0);
  delay(2000); 
}
*/
/*
  analogWrite(PWMpin, 0);
  delay(4000);
  analogWrite(PWMpin, 100);
  delay(4000);
  analogWrite(PWMpin, 150);
  delay(4000);
  analogWrite(PWMpin, 200);
  delay(4000);
  analogWrite(PWMpin, 255);
  delay(4000);
*/
  digitalWrite(motorPin, HIGH);
  delay(200);

}

