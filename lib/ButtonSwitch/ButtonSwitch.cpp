#include <ButtonSwitch.h>

ButtonSwitch::ButtonSwitch(Adafruit_MCP23X17* mcp, int pin, long debounce) {
  this->mcp = mcp;
  this->pin = pin;
  this->debounce = debounce;
}

void ButtonSwitch::init() {
  this->mcp->pinMode(this->pin, INPUT_PULLUP);
}

bool ButtonSwitch::read() {
  if (this->mcp->digitalRead(this->pin) == LOW && millis() - this->lastPress >= this->debounce) {
    this->lastPress = millis();
    return true;
  }
  return false;
}