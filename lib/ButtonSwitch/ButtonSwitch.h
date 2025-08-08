#ifndef ButtonSwitch_H
#define ButtonSwitch_H

#include <Adafruit_MCP23X17.h>

#define DEBOUNCE_SHORT 400
#define DEBOUNCE_LONG  5000

class ButtonSwitch {
public:
  ButtonSwitch(Adafruit_MCP23X17* mcp, int pin, long debounce);
  void init();
  bool read(bool withDebunce = true);
private:
  Adafruit_MCP23X17* mcp;
  int pin;
  long debounce;
  long lastPress = 0;
};

#endif