#include "Scanner.h"

Scanner scanner;

void setup() {
  scanner.attach(9, 8);
}

void loop() {
  scanner.scan();
}
