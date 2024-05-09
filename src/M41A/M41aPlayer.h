#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "AppConstants.h"

//#define _DEBUG


class M41aPlayer {
private:
  /*
  | Start | Version | Length | Command | SetFeedback | Param #1 | Param #2 | ChkSum #1 | ChkSum #2 | End  |
  | 0x7E  | 0xFF    | ..     | ..      | 0x00 / 0x01 | ..       | ..       | ..        | ..        | 0xEF |
  */

  // Commands :
  static const byte CMD_NEXT_SONG            = 0x01; // Příkaz pro přeskočení na další skladbu
  static const byte CMD_PREV_SONG            = 0x02; // Příkaz pro návrat na předchozí skladbu
  static const byte CMD_SPECIFY_TRACK        = 0x03; // Nastavení čísla skldaby - 0 .. 2999
  static const byte CMD_VOLUME_UP            = 0x04; // Zvýšení hlasitosti
  static const byte CMD_VOLUME_DOWN          = 0x05; // Snížení hlasitosti
  static const byte CMD_SET_VOLUME           = 0x06; // Nastavení hlasitosti - 0-30
  static const byte CMD_SET_EQ               = 0x07; // Nastavení ekvalizéru - 0=Normal / 1=Pop / 2=Rock / 3=Jazz / 4=Classic / 5=Base
  static const byte CMD_SET_PLAYBACK_MODE    = 0x08; // Zapnutí opakování přehrání - 0=file / 1=folder / 2=single / 3=random
  static const byte CMD_PLAYBACK_SOURCE      = 0x09; // Nastavení režimu přehrávání - 0=U / 1=TF / 2=AUX / 3=Sleep / 4=Flash
  static const byte CMD_GO_STANDBY           = 0x0A; // Jdi spát, šetři energii
  static const byte CMD_NORMAL_OPERATE       = 0x0B; // Normal working ?
  static const byte CMD_RESET_MODULE         = 0x0C; // Resetovat modul
  static const byte CMD_RESUME               = 0x0D; // Začít přehrávat
  static const byte CMD_PAUSE_PLAY           = 0x0E; // Pozastavit přehrávání
  static const byte CMD_SET_PLAY_FOLDER_FILE = 0x0F; // Nastavit složku k přehrávání - 1-10 (nutno nastavit uživatelem)
  static const byte CMD_VOLUME_ADJUST        = 0x10; // DH=0x01 - open volume adjust, DL=set volume gain 0-31 decimal
  static const byte CMD_REPEAT_PLAY          = 0x11; // 0=Stop repeat play / 1=Start repeat play

  static const byte CMD_STAY0                = 0x3A; // Medium inserted - 1=USB, 2=SD, 3=USB+SD
  static const byte CMD_STAY1                = 0x3B; // Medium removed - 1=USB, 2=SD, 3=USB+SD
  static const byte CMD_STAY2                = 0x3C; // Playback finished
  static const byte CMD_STAY3                = 0x3D; // Playback finished
  static const byte CMD_STAY4                = 0x3E; // ?
  static const byte CMD_INIT                 = 0x3F; // Send init params 0x0F (každý bit reprezentuje jedno zařízení - spodní 4 bity)

  static const byte CMD_ERROR                = 0x40; // Vrátí chybu, vyžádá opakování
  static const byte CMD_ACK                  = 0x41; // ACK
  static const byte CMD_STATUS               = 0x42; // Dotaz na aktuální stav
  static const byte CMD_VOLUME               = 0x43; // Dotaz na aktuální hlasitost
  static const byte CMD_EQ                   = 0x44; // Dotaz na aktuální ekvalizér
  static const byte CMD_PLAYBACK_MODE        = 0x45; // Dotaz na aktuální režim přehrávání
  static const byte CMD_SOFTWARE_VERSION     = 0x46; // Dotaz na aktuální verzi software
  static const byte CMD_TF_CARD_FILES        = 0x47; // Dotaz na celkový počet souborů na TF kartě
  static const byte CMD_UDISK_FILES          = 0x48; // Dotaz na celkový počet souborů na U disku
  static const byte CMD_FLASH_FILES          = 0x49; // Dotaz na celkový počet souborů ve flash paměti
  static const byte CMD_KEEP_ON              = 0x4A; // Ponechat zapnuté
  static const byte CMD_TF_CARD_TRACK        = 0x4B; // Dotaz na aktuální skladbu na TF kartě
  static const byte CMD_UDISK_TRACK          = 0x4C; // Dotaz na aktuální skladbu na U disku
  static const byte CMD_FLASH_TRACK          = 0x4D; // Dotaz na aktuální skladbu ve flash paměti

  static const byte Start_Byte     = 0x7E;
  static const byte Version_Byte   = 0xFF;
  static const byte Command_Length = 0x06;
  static const byte End_Byte       = 0xEF;
  static const byte Acknowledge    = 0x00;

private:
  #define DFPLAYER_RECEIVED_LENGTH 10
  #define DFPLAYER_SEND_LENGTH 10

  // #define TimeOut 0
  // #define WrongStack 1
  // #define DFPlayerCardInserted 2
  // #define DFPlayerCardRemoved 3
  // #define DFPlayerCardOnline 4
  // #define DFPlayerPlayFinished 5
  // #define DFPlayerError 6
  // #define DFPlayerUSBInserted 7
  // #define DFPlayerUSBRemoved 8
  // #define DFPlayerUSBOnline 9
  // #define DFPlayerCardUSBOnline 10
  // #define DFPlayerFeedBack 11

  #define Stack_Header 0
  #define Stack_Version 1
  #define Stack_Length 2
  #define Stack_Command 3
  #define Stack_ACK 4
  #define Stack_Parameter 5
  #define Stack_Parameter1 5
  #define Stack_Parameter2 6
  #define Stack_CheckSum 7
  #define Stack_End 9

  #define DFPLAYER_DEVICE_U_DISK 1
  #define DFPLAYER_DEVICE_SD 2
  #define DFPLAYER_DEVICE_AUX 3
  #define DFPLAYER_DEVICE_SLEEP 4
  #define DFPLAYER_DEVICE_FLASH 5

  #define TimeOut 0
  #define WrongStack 1
  #define DFPlayerCardInserted 2
  #define DFPlayerCardRemoved 3
  #define DFPlayerCardOnline 4
  #define DFPlayerPlayFinished 5
  #define DFPlayerError 6
  #define DFPlayerUSBInserted 7
  #define DFPlayerUSBRemoved 8
  #define DFPlayerUSBOnline 9
  #define DFPlayerCardUSBOnline 10
  #define DFPlayerFeedBack 11

  #define Busy 1
  #define Sleeping 2
  #define SerialWrongStack 3
  #define CheckSumNotMatch 4
  #define FileIndexOut 5
  #define FileMismatch 6
  #define Advertise 7

private:
  unsigned long _timeOutDuration = 500;
  unsigned long _timeOutTimer;
  uint8_t _received[DFPLAYER_RECEIVED_LENGTH];
  uint8_t _sending[DFPLAYER_SEND_LENGTH] = {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
  uint8_t _receivedIndex=0;
  uint8_t _handleType;
  uint8_t _handleCommand;
  uint16_t _handleParameter;
  bool _isAvailable;
  bool _isSending;
  
  // Globální proměnná pro sledování času posledního odeslání příkazu
  uint32_t lastSubmitMillis;
  int lastFireInstrument;
  int lastEmptyMagazineInstrument;

  volatile int volume = 20;
  int eqset;
  SoftwareSerial mySerial;

public:
  M41aPlayer();
  void setup(byte newVolume);
  void update();

public:
  boolean Playing = false;
  byte getVolume();
  void setVolume(byte newVolume);
  void volumeINC();
  void volumeDEC();
  void pause();
  void playFire(int bulletsCount);
  void playReload();
  void playEmptyMagazine();
  void playClick();
  void playVolumeSaved();
  void playVolumeUp();
  void playVolumeDown();
  void playCardInserted();
  void playCardRemoved();
  void playLowBattery();
  void playWeaponReady();

private:
  void printDetail(uint8_t cmd, uint8_t param1, uint8_t param2);
  bool handleMessage(uint8_t type, uint16_t parameter);
  bool handleError(uint8_t type, uint16_t parameter = 0);
  bool waitAvailable(unsigned long duration = 0);
  uint16_t arrayToUint16(uint8_t *array);
  bool validateStack();
  void parseStack();
  bool available();
  void sendStack();
  void enableACK();
  void disableACK();
  uint16_t calculateCheckSum(byte *buffer);
  void initMe();
  void stop();
  void equalizer();
  void exe_cmd(byte CMD, byte Par1, byte Par2);
};
