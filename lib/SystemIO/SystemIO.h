#ifndef SystemIO_H
#define SystemIO_H

#include <../../include/types.h>
#include <Adafruit_MCP23X17.h>
#include <TM1637Display.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <ButtonSwitch.h>

#define N_MCPs 3

#define MCP_INDEX_INPUT   0
#define MCP_INDEX_OUTPUT  1

#define INPUT_MODE_PIN      0
#define INPUT_DIRECTION_N   1
#define INPUT_DIRECTION_S   2
#define INPUT_DIRECTION_E   3
#define INPUT_DIRECTION_W   4
#define INPUT_MASTER_ALARM  5
#define INPUT_ENGINE_ARM    6
#define INPUT_O2_STIR       8
#define INPUT_LAUNCH        9
#define INPUT_LAND          10

#define ANALOG_INPUT_THROTTLE 26

#define OUTPUT_FUEL_LIGHT     0
#define OUTPUT_MASTER_ALARM   1
#define OUTPUT_CONTACT_LIGHT  2
#define OUTPUT_ENGINE_LIGHT_1 3
#define OUTPUT_ENGINE_LIGHT_2 4
#define OUTPUT_ENGINE_LIGHT_3 5
#define OUTPUT_ENGINE_LIGHT_4 6
#define OUTPUT_ENGINE_LIGHT_5 7

#define DISPLAY_CLK_ALT   18
#define DISPLAY_DIO_ALT   19
#define DISPLAY_CLK_FUEL  20
#define DISPLAY_DIO_FUEL  21

#define TFT_CS        10
#define TFT_RST        12 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         11

class SystemIO {
public:
  bool init();
  void reset();
  
  t_mode getModeSelection();
  t_direction getDirection();
  double getThrottle();
  bool getMasterAlarm();
  bool getEngineArm();
  bool getO2Stir();
  bool getLaunch();
  bool getLand();

  void setAltitude(int v);
  void setFuel(int f);
  void setFuelLight(bool b);
  void setMasterAlarm(bool b);
  void setContactLight(bool b);
  void setEngineLights(bool *b);

  void playTrack(int track);

  Adafruit_ST7735* getTFT();
private:
  Adafruit_MCP23X17* mcps[N_MCPs];
  TM1637Display* altitude;
  TM1637Display* fuel;
  Adafruit_ST7735* tft;
  ButtonSwitch *directionN;
  ButtonSwitch *directionS;
  ButtonSwitch *directionE;
  ButtonSwitch *directionW;
  ButtonSwitch *masterAlarm;
  ButtonSwitch *launch;
  ButtonSwitch *land;
};

#endif