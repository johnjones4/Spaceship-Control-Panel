#ifndef ModeLand_H
#define ModeLand_H

#include <Mode.h>
#include <../include/types.h>

#define DELTA_X_INITIAL 1.0
#define DELTA_Y_INITIAL 1.0
#define THRUST_INITIAL 0.0
#define MAX_THRUST 4.0
#define DIRECTION_DELTA_DELTA 0.1
#define MIN_ABS_DELTA 0.1
#define GRAVITY -1.62
#define ALTITUDE_INITIAL 9000.0
#define ALTITUDE_MIN 3.0
#define FUEL_INITIAL 1000.0
#define FUEL_USAGE_RATE -1.0
#define FUEL_MIN 100

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
  long start;
  long lastStep;
};

#endif