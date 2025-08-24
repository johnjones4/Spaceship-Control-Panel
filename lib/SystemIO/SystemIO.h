#ifndef SystemIO_H
#define SystemIO_H

#include <../../include/types.h>
#include <Adafruit_MCP23X17.h>
#include <TM1637Display.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <ButtonSwitch.h>
#include <Adafruit_Soundboard.h>
#include <HardwareSerial.h>

#define N_MCPs 2

#define MCP_INDEX_INPUT   0
#define MCP_INDEX_OUTPUT  1

#define INPUT_MODE_FREE       0
#define INPUT_MODE_DOCK       1
#define INPUT_MODE_LAND       2
#define INPUT_DIRECTION_N     3
#define INPUT_DIRECTION_S     4
#define INPUT_DIRECTION_E     5
#define INPUT_DIRECTION_W     6
#define INPUT_MASTER_ALARM    7
#define INPUT_O2_STIR         9
#define INPUT_ENGINE_ARM      8
#define INPUT_TOWER_JET       10
#define INPUT_MAIN_CHUTE      11
#define INPUT_LAUNCH          15
#define INPUT_LAND            14
#define INPUT_RETRACT         12
#define INPUT_RESET           13

#define ANALOG_INPUT_THROTTLE 26

#define OUTPUT_MASTER_ALARM   0
#define OUTPUT_FUEL_LIGHT     1
#define OUTPUT_CONTACT_LIGHT  2
#define OUTPUT_DOCKED         3
#define OUTPUT_ENGINE_LIGHT_1 4
#define OUTPUT_ENGINE_LIGHT_2 5
#define OUTPUT_ENGINE_LIGHT_3 6
#define OUTPUT_ENGINE_LIGHT_4 7
#define OUTPUT_ENGINE_LIGHT_5 8
#define OUTPUT_THRUSTER_N     9
#define OUTPUT_THRUSTER_S     10
#define OUTPUT_THRUSTER_E     11
#define OUTPUT_THRUSTER_W     12

#define DISPLAY_CLK_ALT   18
#define DISPLAY_DIO_ALT   19
#define DISPLAY_CLK_FUEL  14
#define DISPLAY_DIO_FUEL  15

#define TFT_SCLK  2
#define TFT_MOSI  3
#define TFT_CS    10
#define TFT_RST   11
#define TFT_DC    12

#define SFX_TX  8
#define SFX_RX  9
#define SFX_RST 7

class SystemIO {
public:
  bool init();
  void reset();
  void step();
  
  t_mode getModeSelection();
  t_direction getDirection();
  double getThrottle();
  bool getMasterAlarm();
  bool getEngineArm();
  bool getO2Stir();
  bool getLaunch();
  bool getLand();
  bool getRetract();
  bool getReset();
  bool getTowerJet();
  bool getMainChute();

  void setAltitude(int v);
  void setFuel(int f);
  void setFuelLight(bool b);
  void setMasterAlarm(bool b);
  void setContactLight(bool b);
  void setEngineLights(uint8_t *b);

  void playTrack(uint8_t track);

  Adafruit_ST7735* getTFT();
private:
  Adafruit_MCP23X17* mcps[N_MCPs];
  Adafruit_Soundboard *sfx;
  HardwareSerial* sfxSerial;
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
  ButtonSwitch *retract;
  ButtonSwitch *resetb;
  bool masterAlarmState;
  uint8_t* engineLightsStatus = NULL;
};

#endif