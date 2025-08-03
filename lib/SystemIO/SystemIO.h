#ifndef SystemIO_H
#define SystemIO_H

#include <../include/types.h>

class SystemIO {
public:
  bool init();
  
  t_mode getModeSelection();
  t_direction getDirection();
  double getThrottle();
  bool getMasterAlarm();

  void setAltitude(int v);
  void setFuel(int f);
  void setFuelLight(bool b);
  void setMasterAlarm(bool b);
  void setContactLight(bool b);

  void playTrack(int track);
};

#endif