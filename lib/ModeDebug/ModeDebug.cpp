#include <ModeDebug.h>

ModeDebug::ModeDebug(SystemIO* systemIo) : Mode(systemIo) {

}

void ModeDebug::reset() {

}

void ModeDebug::step() {
  Serial.println("beep");
  delay(1000);
  Serial.print("Joystick: ");
  Serial.println(this->systemIo->getDirection());
}