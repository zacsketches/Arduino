/* Written by Hayden Staples, Aug 2013

   Reads an input voltage on A0 and then coverts this reading back into 
   a voltage value based on the idea that an Arduino has a 10 bit ADC
   that gives a precision of 1024 step values.
*/

int inputPin = 0;

void setup()
{
  Serial.begin(9600);
}
void loop()
{
  int inputRead = analogRead(inputPin);
  float voltage = inputRead * 0.00488;
  Serial.print("THE VOLTAGE IS " );
  Serial.println(voltage);
}
