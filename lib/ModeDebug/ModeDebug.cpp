#include <ModeDebug.h>

ModeDebug::ModeDebug(SystemIO* systemIo) : Mode(systemIo) {

}

void ModeDebug::reset() {

}

void ModeDebug::step() {
  Serial.println("-----------------------");
  Serial.println(systemIo->getO2Stir());
  Serial.println(systemIo->getEngineArm());
  Serial.println(systemIo->getTowerJet());
  Serial.println(systemIo->getMainChute());
}