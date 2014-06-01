void setup()
{
  Serial.begin(9600);
}
byte a=43;
void loop()
{
   Serial.write(a);
   delay(250);
 }
