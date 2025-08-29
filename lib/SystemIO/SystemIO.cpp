#include <SystemIO.h>
#include <Wire.h>

// #define PRINT_OUTPUT

bool SystemIO::init() {
  this->sfxSerial = new UART(SFX_TX, SFX_RX);
  this->sfxSerial->begin(9600);
  this->sfx = new Adafruit_Soundboard(this->sfxSerial, NULL, SFX_RST);
  if (!this->sfx->reset()) {
    Serial.println("Sound board not found!");
    return false;
  } else {
    Serial.println("Sound board found");
  }

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

  int inputs[7] = {
    INPUT_MODE_FREE,
    INPUT_MODE_LAND,
    INPUT_MODE_DOCK,
    INPUT_ENGINE_ARM,
    INPUT_O2_STIR,
    INPUT_TOWER_JET,
    INPUT_MAIN_CHUTE,
  };
  for (int i = 0; i < 7; i++) {
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
  this->retract = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_RETRACT, DEBOUNCE_SHORT);
  this->resetb = new ButtonSwitch(this->mcps[MCP_INDEX_INPUT], INPUT_RESET, DEBOUNCE_SHORT);
  ButtonSwitch* toggles[9] = {
    this->directionN,
    this->directionS,
    this->directionE,
    this->directionW,
    this->masterAlarm,
    this->launch,
    this->land,
    this->retract,
    this->resetb,
  };
  for (int i = 0; i < 9; i++) {
    toggles[i]->init();
  }

  this->altitude = new TM1637Display(DISPLAY_CLK_ALT, DISPLAY_DIO_ALT);
  this->altitude->setBrightness(0xff);
  this->altitude->clear();
  this->fuel = new TM1637Display(DISPLAY_CLK_FUEL, DISPLAY_DIO_FUEL);
  this->fuel->setBrightness(0xff);
  this->fuel->clear();

  this->masterAlarmState = false;
  uint8_t b[5] = {0,0,0,0,0};
  this->setEngineLights(b);

  return true;
}

void SystemIO::reset() {
  this->setAltitude(9999);
  this->setFuel(9999);
  this->setFuelLight(true);
  this->setMasterAlarm(true);
  this->setContactLight(true);
  uint8_t b[5] = {1,1,2,1,1};
  this->setEngineLights(b);
  this->tft->fillScreen(ST77XX_BLACK);
  delay(1000);
  this->setAltitude(0);
  this->setFuel(0);
  this->setFuelLight(false);
  this->setMasterAlarm(false);
  this->setContactLight(false);
  uint8_t b1[5] = {0,0,0,0,0};
  this->setEngineLights(b1);
  this->tft->fillScreen(ST77XX_WHITE);
  this->stopTrack();
}
  
t_mode SystemIO::getModeSelection() {
  if (this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_MODE_DOCK) == LOW) {
    return Dock;
  } else if (this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_MODE_LAND) == LOW) {
    return Land;
  } else {
    return Freeplay;
  }
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
  return this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_O2_STIR) == LOW;
}

bool SystemIO::getLaunch() {
  return this->launch->read();
}

bool SystemIO::getLand() {
  return this->land->read();
}

bool SystemIO::getRetract() {
  return this->retract->read();
}

bool SystemIO::getReset() {
  return this->resetb->read();
}

bool SystemIO::getTowerJet() {
  return this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_TOWER_JET) == LOW;
}

bool SystemIO::getMainChute() {
  return this->mcps[MCP_INDEX_INPUT]->digitalRead(INPUT_MAIN_CHUTE) == LOW;
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
  this->masterAlarmState = b;
}

void SystemIO::setContactLight(bool b) {
#ifdef PRINT_OUTPUT
  Serial.print("Contact Light: ");
  Serial.println(b);
#endif
  this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(OUTPUT_CONTACT_LIGHT, b ? HIGH : LOW);
}

void SystemIO::setEngineLights(uint8_t *b) {
  if (this->engineLightsStatus == NULL) {
    this->engineLightsStatus = (uint8_t*)malloc(sizeof(uint8_t) * 5);
  }
  memcpy(this->engineLightsStatus, b, 5 * sizeof(uint8_t));
  for (int i = 0; i < 5; i++) {
#ifdef PRINT_OUTPUT
    Serial.print("Engine Light: ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(b[i]);
#endif
  }
}

void SystemIO::playTrack(uint8_t track) {
#ifdef PRINT_OUTPUT
  Serial.print("Playing track: ");
  Serial.println(track);
#endif
  this->sfx->playTrack((uint8_t)track);
  this->sfx->playTrack((uint8_t)track);
}

void SystemIO::stopTrack() {
  this->sfx->stop();
}

Adafruit_ST7735* SystemIO::getTFT() {
  return this->tft;
}

void SystemIO::step() {\
  unsigned long now = millis();
  bool blinkOn = (now / 1000) % 2 == 0;
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

  this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(OUTPUT_MASTER_ALARM, this->masterAlarmState && blinkOn ? HIGH : LOW);

  if (this->engineLightsStatus != NULL) {
    int pins[5] = {
      OUTPUT_ENGINE_LIGHT_1,
      OUTPUT_ENGINE_LIGHT_2,
      OUTPUT_ENGINE_LIGHT_3,
      OUTPUT_ENGINE_LIGHT_4,
      OUTPUT_ENGINE_LIGHT_5
    };
    for (int i = 0; i < 5; i++) {
      this->mcps[MCP_INDEX_OUTPUT]->digitalWrite(pins[i], this->engineLightsStatus[i] == 1 || (this->engineLightsStatus[i] == 2 && blinkOn) ? HIGH : LOW);
    }
  }
}