#pragma once

#include <Arduino.h>
#include "AppConstants.h"
#include "DFPlay.h"

class M41aPlayer {
private:
  DFPlay dfPlay;
  int lastFireInstrument;
  int lastEmptyMagazineInstrument;

private:
  volatile int volume = 20;

public:
  M41aPlayer();
  void setup(byte newVolume);
  void update();

public:
  bool getPlaying();
  byte getVolume();
  void setVolume(byte newVolume);
  void volumeIncrement();
  void volumeDecrement();
  void pause();
  void stop();
  void setEqualizer(byte equalizerType);

public:
  void playFire(int bulletsCount);
  void playCock();
  void playMagazineReload();
  void playMagazineEject();
  void playMagazineLoad();
  void playEmptyMagazine();
  void playClick();
  void playVolumeSaved();
  void playVolumeUp();
  void playVolumeDown();
  void playCardInserted();
  void playCardRemoved();
  void playLowBattery();
  void playWeaponReady();
};
