#pragma once

#include <Arduino.h>
#include "M41aPlayer.h"
#include "TwoSegmentDisplay.h"
#include "GranadeLedAnimator.h"


//# define ACTIVATED LOW

class M41aSimulator {
private:  
  const int button1_Pin = A2; // spoušť puška
  const int button2_Pin = A3; // zásobník výměna
  const int button3_Pin = A4; // natáhnout závěr
  const int button4_Pin = A5; // spoušť granátomet / nabít granátomet
  const int button5_Pin = A6; // reserved

  const int rotaryS1_Pin = 9; // volume encoder
  const int rotaryS2_Pin = 10; // volume encoder
  const int rotaryButton_Pin = A1; // rotary encoder button

  const int led_Pin = A0; 
  const int ledGranade_Pin = 5; // must be PWM
  
  // Define Connections to 74HC595
  const int latch_Pin = 12;  //74HC595 RCLK  pin 12 ST_CP (LAOD) - zelený 
  const int clock_Pin = 11;  //74HC595 SRCLK pin 11 SH_CP (SCLK) - žlutý
  const int data_Pin = 13;   //74HC595 SER   pin 14 DS    (SDD)  - modrý

  const int VolumeDisplayTimeout = 3000;

private:
  int lastButton1_State;
  int lastButton2_State;
  int lastButton3_State;
  int lastButton4_State;
  int lastButtonVolume_State;

  // Proměnné pro uchování posledního stavu
  volatile unsigned long lastVolumeEncoderRun;
  volatile int newVolume;
  bool useSoftVolumeRotaryEncoder;
  int oldRotaryMasterValue;
  bool ledState;
  int rotaryMasterValue;
  int displayTime;
  int bulletsCount;
  int lastBulletsCount;
  unsigned long fireMillis;
  unsigned long ledBlinkMillis;
  bool volumeDisplayed;
  bool weaponReadyPlayed;
  bool button1IsPressed;
  bool button2IsPressed;
  bool button3IsPressed;
  bool button4IsPressed;
  bool buttonVolumeIsPressed;
  bool isVolumeEpromWriteFlag;
  bool isGranadeLoaded;

private:
  M41aPlayer player;
  TwoSegmentDisplay display;
  GranadeLedAnimator granadeLedAnimator;
  void (*afterPlaybackDelegate)() = nullptr;

public:
  M41aSimulator();
  void setup();
  void update();

private:
  //void doInterruptVolumeRotaryEncoder();
  void doInternalVolumeDecisions();
  void ledTurn();
  void ledOff();
  //
  void doButton1FireRifle();
  void doButton2MagazineReload();
  void doButton3Cock();
  void doButton4GranadeRelaodOrFire();
  //
  void granadeLoad();
  void granadeFire();
  void granadeExplosion();
  //
  void cock();
  //
  void magazineReload();
  void magazineEject();
  void magazineLoad();
  //
  void doVolumeButton();
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