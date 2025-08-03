#include <Arduino.h>
#include <Application.h>

Application app;

void setup() {
  app.init();
}

void loop() {
  app.step();
}
