#ifndef Mode_H
#define Mode_H

#include <SystemIO.h>

class Mode {
public:
  Mode(SystemIO* systemIo);
  virtual void reset() = 0;
  virtual void step() = 0;
protected:
  SystemIO* systemIo;
};

#endif