#include <Application.h>
#include <Arduino.h>
#include <ModeLand.h>
#include <ModeDebug.h>
#include <ModeFree.h>

// #define DEBUG_MODE

void Application::init() {
  Serial.begin(9600);
#ifdef DEBUG_MODE
  delay(5000);
#endif
  Serial.println("starting");

  randomSeed(analogRead(0));

  this->systemIO = new SystemIO();
  if (!this->systemIO->init()) {
    Serial.println("Error starting system IO");
    while (true) {}
  }
}

void Application::step() {
#ifdef DEBUG_MODE
  if (this->currentMode == NULL) {
    this->systemIO->reset();
    this->currentMode = new ModeDebug(this->systemIO);
    this->currentMode->reset();
  }
#else
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
#endif
  this->currentMode->step();
}

Mode* Application::modeFactory(t_mode modeType) {
  switch (modeType)
  {
  case Land:
    return new ModeLand(this->systemIO);
  case Freeplay:
    return new ModeFree(this->systemIO);
  default:
    return NULL;
  }
}