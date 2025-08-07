#include <ButtonSwitch.h>

#define PRINT_INPUT

ButtonSwitch::ButtonSwitch(Adafruit_MCP23X17* mcp, int pin, long debounce) {
  this->mcp = mcp;
  this->pin = pin;
  this->debounce = debounce;
}

void ButtonSwitch::init() {
  this->mcp->pinMode(this->pin, INPUT_PULLUP);
}

bool ButtonSwitch::read() {
#ifdef PRINT_INPUT
  Serial.print("MCP Input ");
  Serial.print(pin);
  Serial.print(" ");
  Serial.println(this->mcp->digitalRead(this->pin) == LOW);
#endif
  if (this->mcp->digitalRead(this->pin) == LOW && millis() - this->lastPress >= this->debounce) {
    this->lastPress = millis();
    return true;
  }
  return false;
}