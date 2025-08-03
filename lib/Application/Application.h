#ifndef Application_h
#define Application_h

#include <SystemIO.h>
#include <Mode.h>
#include <../include/types.h>

class Application {
public:
  void init();
  void step();
private:
  Mode* modeFactory(t_mode modeType);
  SystemIO *systemIO;
  Mode *currentMode;
  t_mode currentModeType;
};

#endif