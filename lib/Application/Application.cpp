#include <Application.h>
#include <Arduino.h>

void Application::init() {
  Serial.begin(9600);
  //TODO
  //Setup systemio
  //Setup modes
}

void Application::step() {
  t_mode nextMode = this->systemIO->getModeSelection();
  if (this->currentMode == NULL || nextMode != this->currentModeType) {
    this->currentMode = this->modeFactory(nextMode);
    this->currentModeType = nextMode;
    this->currentMode->reset();
  }
  this->currentMode->step();
}