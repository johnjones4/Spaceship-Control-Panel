#ifndef ModeLand_H
#define ModeLand_H

#include <Mode.h>
#include <../../include/types.h>

#define DELTA_X_INITIAL 5.0
#define DELTA_Y_INITIAL 1.0
#define THRUST_INITIAL 0.0
#define MAX_THRUST 4.0
#define DIRECTION_DELTA_DELTA 0.5
#define MIN_ABS_DELTA 3
#define GRAVITY -1.62
#define ALTITUDE_INITIAL 1000.0
#define ALTITUDE_MIN 1.0
#define ALTITUDE_LIGHT_MIN 3.0
#define FUEL_INITIAL 2000.0
#define FUEL_USAGE_RATE -10
#define FUEL_MIN 100
#define MAX_VELOCITY_X 10.0
#define MAX_VELOCITY_Y 10.0
#define RANDOM_VELOCITY_CHANGE_MAX 0.25

class ModeLand : public Mode {
public:
  ModeLand(SystemIO* systemIo);
  void reset();
  void step();
private:
  double deltaX;
  double deltaY;
  double deltaZ;
  double thrust;
  double altitude;
  double fuel;
  unsigned long start;
  unsigned long lastStep;
  int16_t lastXLine = -1;
  int16_t lastYLine = -1;

  int16_t cartesienPlotCoord(double max, double value, int16_t length);
};

#endif