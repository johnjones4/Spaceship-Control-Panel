#include <SystemIO.h>
#include <Wire.h>

// #define PRINT_OUTPUT

bool SystemIO::init() {
  this->tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
  this->tft->initR(INITR_BLACKTAB);

  for (int i = 0; i < N_MCPs; i++) {
    Serial.println("Initializing MCP");
    this->mcps[i] = new Adafruit_MCP23X17();
    if (!this->mcps[i]->begin_I2C(MCP23XXX_ADDR + i)) {
      Serial.print("Failed to start MCP:");
      Serial.println(i);
      return false;
    }
  }

  int inputs[5] = {
    INPUT_MODE_FREE,
    INPUT_MODE_LAND,
    INPUT_MODE_DOCK,
    INPUT_ENGINE_ARM,
    INPUT_O2_STIR,
  };
  for (int i = 0; i < 5; i++) {
    this->mcps[MCP_INDEX_INPUT]->pinMode(inputs[i], INPUT_PULLUP);
  }
  pinMode(ANALOG_INPUT_THROTTLE, INPUT);

  int outputs[12] = {
    OUTPUT_FUEL_LIGHT,
    OUTPUT_MASTER_ALARM,
    OUTPUT_CONTACT_LIGHT,
    OUTPUT_ENGINE_LIGHT_1,
    OUTPUT_ENGINE_LIGHT_2,
    OUTPUT_ENGINE_LIGHT_3,
    OUTPUT_ENGINE_LIGHT_4,
    OUTPUT_ENGINE_LIGHT_5,
    OUTPUT_THRUSTER_N,
    OUTPUT_THRUSTER_S,
    OUTPUT_THRUSTER_E,
    OUTPUT_THRUSTER_W
  };
  for (int i = 0; i < 12; i++) {
    this->mcps[MCP_INDEX_OUTPUT]->pinMode(outputs[i], OUTPUT);
  }

  this->directionN = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_DIRECTION_N, DEBOUNCE_SHORT);
  this->directionS = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_DIRECTION_S, DEBOUNCE_SHORT);
  this->directionE = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_DIRECTION_E, DEBOUNCE_SHORT);
  this->directionW = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_DIRECTION_W, DEBOUNCE_SHORT);
  this->masterAlarm = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_MASTER_ALARM, DEBOUNCE_SHORT);
  this->launch = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_LAUNCH, DEBOUNCE_SHORT);
  this->land = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_LAND, DEBOUNCE_SHORT);
  ButtonSwitch* toggles[7] = {
    this->directionN,
    this->directionS,
    this->directionE,
    this->directionW,
    this->masterAlarm,
    this->launch,
    this->land,
  };
  for (int i = 0; i < 7; i++) {
    toggles[i]->init();
  }

  this->altitude = new TM1637Display(DISPLAY_CLK_ALT, DISPLAY_DIO_ALT);
  this->altitude->setBrightness(0xff);
  this->altitude->clear();
  this->fuel = new TM1637Display(DISPLAY_CLK_FUEL, DISPLAY_DIO_FUEL);
  this->fuel->setBrightness(0xff);
  this->fuel->clear();

  return true;
}

void SystemIO::reset() {
  this->setAltitude(9999);
  this->setFuel(9999);
  this->setFuelLight(true);
  this->setMasterAlarm(true);
  this->setContactLight(true);
  this->tft->fillScreen(ST77XX_BLACK);
  delay(1000);
  this->setAltitude(0);
  this->setFuel(0);
  this->setFuelLight(false);
  this->setMasterAlarm(false);
  this->setContactLight(false);
  this->tft->fillScreen(ST77XX_WHITE);
}
  
t_mode SystemIO::getModeSelection() {
  return Land; //TODO
}

t_direction SystemIO::getDirection() {
  ButtonSwitch *inputs[4] = {
    this->directionN,
    this->directionS,
    this->directionE,
    this->directionW,
  };
  t_direction directions[4] = {
    North,
    South,
    East,
    West
  };
  for (int i = 0; i < 4; i++) {
    if (inputs[i]->read()) {
      return directions[i];
    }
  }
  return Neutral;
}

double SystemIO::getThrottle() {
  return (double)analogRead(ANALOG_INPUT_THROTTLE) / 1024.0;
}

bool SystemIO::getMasterAlarm() {
  return this->masterAlarm->read();
}

bool SystemIO::getEngineArm() {
  return this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_ENGINE_ARM) == LOW;
}

bool SystemIO::getO2Stir() {
  return this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_ENGINE_ARM) == LOW;
}

bool SystemIO::getLaunch() {
  return this->launch->read();
}

bool SystemIO::getLand() {
  return this->land->read();
}

void SystemIO::setAltitude(int v) {
#ifdef PRINT_OUTPUT
  Serial.print("Altitude: ");
  Serial.println(v);
#endif
  this->altitude->showNumberDec(v, false);
}

void SystemIO::setFuel(int f) {
#ifdef PRINT_OUTPUT
  Serial.print("Fuel: ");
  Serial.println(f);
#endif
  this->fuel->showNumberDec(f, false);
}

void SystemIO::setFuelLight(bool b) {
#ifdef PRINT_OUTPUT
  Serial.print("Fuel Light: ");
  Serial.println(b);
#endif
  this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(OUTPUT_FUEL_LIGHT, b ? HIGH : LOW);
}

void SystemIO::setMasterAlarm(bool b) {
#ifdef PRINT_OUTPUT
  Serial.print("Master Alarm: ");
  Serial.println(b);
#endif
  this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(OUTPUT_MASTER_ALARM, b ? HIGH : LOW);
}

void SystemIO::setContactLight(bool b) {
#ifdef PRINT_OUTPUT
  Serial.print("Contact Light: ");
  Serial.println(b);
#endif
  this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(OUTPUT_CONTACT_LIGHT, b ? HIGH : LOW);
}

void SystemIO::setEngineLights(bool *b) {
  int pins[5] = {
    OUTPUT_ENGINE_LIGHT_1,
    OUTPUT_ENGINE_LIGHT_2,
    OUTPUT_ENGINE_LIGHT_3,
    OUTPUT_ENGINE_LIGHT_4,
    OUTPUT_ENGINE_LIGHT_5
  };
  for (int i = 0; i < 5; i++) {
#ifdef PRINT_OUTPUT
    Serial.print("Contact Light: ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(b[i]);
#endif
    this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(pins[i], b[i] ? HIGH : LOW);
  }
}

void SystemIO::playTrack(int track) {
#ifdef PRINT_OUTPUT
  Serial.print("Playing track: ");
  Serial.println(track);
#endif
}

Adafruit_ST7735* SystemIO::getTFT() {
  return this->tft;
}

void SystemIO::step() {
  ButtonSwitch *inputs[4] = {
    this->directionN,
    this->directionS,
    this->directionE,
    this->directionW,
  };
  int outputs[4] = {
    OUTPUT_THRUSTER_N,
    OUTPUT_THRUSTER_S,
    OUTPUT_THRUSTER_E,
    OUTPUT_THRUSTER_W
  };
  for (int i = 0; i < 4; i++) {
    this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(outputs[i], inputs[i]->read(false));
  }
}