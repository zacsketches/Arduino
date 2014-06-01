//Scan Control Constants
const byte SCAN_REQUESTED = B0001;
const byte MEASUREMENT_READY = B0010;
const byte ACK_RECEIVED = B0100;
const byte SCAN_COMPLETE = B1000;

void setup() {
  Serial.begin(9600);
  byte ScanState = ACK_RECEIVED | MEASUREMENT_READY;
  Serial.println();
  Serial.print("ScanState is: ");
  Serial.println(ScanState, BIN);
}
void loop() {
}
