//This will make the motor buzz 3 times if something gets close enough
int motorPin = 5;

void setup()
{
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, HIGH);
  digitalWrite(motorPin, LOW);
  digitalWrite(motorPin, HIGH);
  digitalWrite(motorPin, LOW);
  digitalWrite(motorPin, HIGH);
}
void loop()
{
  delay(100);
}
