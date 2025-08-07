#ifndef ModeDebug_H
#define ModeDebug_H

#include <Mode.h>

class ModeDebug : public Mode {
public:
  ModeDebug(SystemIO* systemIo);
  void reset();
  void step();
};

#endif