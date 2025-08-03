#ifndef SystemIO_H
#define SystemIO_H

#include <../include/types.h>

class SystemIO {
public:
  t_mode getModeSelection();
  t_direction getDirection();
  double getThrottle();

  void setVelocity(int v);
  void setFuel(int f);
};

#endif