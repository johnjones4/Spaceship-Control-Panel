#ifndef ModeFree_H
#define ModeFree_H

#include <Mode.h>

#define NO_TRACK 255

class ModeFree : public Mode {
public:
  ModeFree(SystemIO* systemIo);
  void reset();
  void step();
private:
  unsigned long lastAudioStart = 0;
  uint8_t trackPlaying = NO_TRACK;
  

  void handlePlayingTrack();
  void handleNextPlayingTrack();
};

#endif