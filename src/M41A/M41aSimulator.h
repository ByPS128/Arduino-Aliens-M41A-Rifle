#pragma once

#include <Arduino.h>
#include "M41aPlayer.h"
#include "TwoDigitDisplay.h"
#include "RifleLedAnimator.h"
#include "GranadeLedAnimator.h"
#include "DisplayAnimator.h"
#include "Button.h"
#include "LongPressButton.h"


//# define ACTIVATED LOW

class M41aSimulator {
private:  
  const int button1_Pin = A2; // rifle trigger
  const int button2_Pin = A3; // magazine exchnage
  const int button3_Pin = A4; // cock the gun
  const int button4_Pin = A5; // grenade launcher trigger / load grenade launcher
  const int button5_Pin = A6; // reserved

  const int rotaryS1_Pin = 9; // volume rotary encoder (S1)
  const int rotaryS2_Pin = 10; // volume rotary encoder (S2)
  const int rotaryButton_Pin = 8; // volume rotary encoder button (KEY)

  const int ledRiffle_Pin = A0; // Led of riffle barrel
  const int ledGranade_Pin = 5; // Led of granade barrel, must be PWM because of animation
  
  // Define Connections to 74HC595
  const int latch_Pin = 12;  //74HC595 RCLK   ST_CP (LAOD)
  const int clock_Pin = 11;  //74HC595 SRCLK  SH_CP (SCLK)
  const int data_Pin = 13;   //74HC595 SER    DS    (SDD) 

  const int VolumeDisplayTimeout = 3000; // Time in milliseconds for how long it will take to display the volume.

private:
  unsigned long lastVolumeEncoderRun;
  int newVolume;
  int oldRotaryMasterValue;
  int rotaryMasterValue;
  int displayTime;
  int bulletsCount;
  int lastBulletsCount;
  unsigned long fireMillis;
  unsigned long ledBlinkMillis;
  bool volumeDisplayed;
  bool weaponReadyPlayed;
  bool isVolumeEpromWriteFlag;
  bool isGranadeLoaded;

private:
  Button button1;
  Button button2;
  Button button3;
  Button button4;
  LongPressButton rotaryButton;
  M41aPlayer player;
  TwoDigitDisplay display;
  RifleLedAnimator rifleLedAnimator;
  GranadeLedAnimator granadeLedAnimator;
  DisplayAnimator displayAnimator;
  void (*afterPlaybackDelegate)() = nullptr;

public:
  M41aSimulator();
  void setup();
  void update();

private:
  static void onAnimationCompletecCallback(M41aSimulator* instance);
  static void rifleFireCallback(M41aSimulator* instance);
  static void rifleStopCallback(M41aSimulator* instance);
  static void cockCallback(M41aSimulator* instance);
  static void magazineReloadCallback(M41aSimulator* instance);
  static void grandeFireCallback(M41aSimulator* instance);
  static void volumeButtonShortPressCallback(M41aSimulator* instance);
  static void volumeButtonLongPressCallback(M41aSimulator* instance);

private:
  void doInternalVolumeDecisions();
  //
  void doButton1FireRifle();
  void doButton2MagazineReload();
  void doButton3Cock();
  void doButton4GranadeRelaodOrFire();
  //
  void granadeLoadOrFire();
  void granadeLoad();
  void granadeFire();
  void granadeExplosion();
  //
  void rifleFire();
  void rifleStop();
  void cock();
  void magazineReload();
  void magazineEject();
  void magazineLoad();
  //
  void volumeButtonShortPress();
  void volumeButtonLongPress();
  //
  void processVolume();
  void doSoftVolumeRotaryEncoder();
  void displayBullets();
  void displayVolume();
  //
  bool readIsVolumeEpromWriteFlag();
  byte readVolumeFromEprom();
  void writeVolumeToEprom(byte volume);
};