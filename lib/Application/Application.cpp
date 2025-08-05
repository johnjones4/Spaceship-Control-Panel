#include <Application.h>
#include <Arduino.h>
#include <ModeLand.h>

void Application::init() {
  Serial.begin(9600);

  this->systemIO = new SystemIO();
  if (!this->systemIO->init()) {
    Serial.println("Error starting system IO");
    while (true) {}
  }
}

void Application::step() {
  t_mode nextMode = this->systemIO->getModeSelection();
  if (this->currentMode == NULL || nextMode != this->currentModeType) {
    this->systemIO->reset();
    if (this->currentMode != NULL) {
      free(this->currentMode);
    }
    this->currentMode = this->modeFactory(nextMode);
    this->currentModeType = nextMode;
    this->currentMode->reset();
  }
  this->currentMode->step();
}

Mode* Application::modeFactory(t_mode modeType) {
  switch (modeType)
  {
  case Land:
    return new ModeLand(this->systemIO);
  default:
    return NULL;
  }
}