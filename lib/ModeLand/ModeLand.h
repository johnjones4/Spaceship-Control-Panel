#ifndef ModeLand_H
#define ModeLand_H

#include <Mode.h>

class ModeLand : public Mode {
public:
  ModeLand(SystemIO* systemIo);
  void reset();
  void step();
};

#endif