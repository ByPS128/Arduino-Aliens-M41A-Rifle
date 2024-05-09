#pragma once

#include <Arduino.h>
#include "M41aPlayer.h"
#include "TwoSegmentDisplay.h"

//# define ACTIVATED LOW

class M41aSimulator {
private:  
  const int button1_Pin = 4; // spoušť puška
  const int button2_Pin = 5; // přebít
  const int button3_Pin = 6; // granátomet tam
  const int button4_Pin = 7; // granátomet zpět
  const int button5_Pin = 8; // zásobník výměna 

  const int rotaryS1_Pin = 9; // volume encoder
  const int rotaryS2_Pin = 10; // volume encoder
  const int rotaryButton_Pin = A1; // rotary encoder button

  const int led_Pin = A0; 
  
  // Define Connections to 74HC595
  const int latch_Pin = 12;  //74HC595 RCLK  pin 12 ST_CP (LAOD) - zelený 
  const int clock_Pin = 11;  //74HC595 SRCLK pin 11 SH_CP (SCLK) - žlutý
  const int data_Pin = 13;   //74HC595 SER   pin 14 DS    (SDD)  - modrý

  const int VolumeDisplayTimeout = 3000;

private:
  int lastButton1_State;
  int lastButton2_State;
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
  bool buttonVolumeIsPressed;
  bool isVolumeEpromWriteFlag;

private:
  M41aPlayer player;
  TwoSegmentDisplay display;
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
  void doButton1();
  void doButton2();
  void reloadMagazine();
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