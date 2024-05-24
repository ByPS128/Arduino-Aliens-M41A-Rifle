#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "AppConstants.h"

//#define _DEBUG

class DFPlay {
private:
  /*
  | Start | Version | Length | Command | SetFeedback | Param #1 | Param #2 | ChkSum #1 | ChkSum #2 | End  |
  | 0x7E  | 0xFF    | ..     | ..      | 0x00 / 0x01 | ..       | ..       | ..        | ..        | 0xEF |
  */

  // Commands :
  static const byte CMD_NEXT_SONG            = 0x01; // Command to skip to the next song
  static const byte CMD_PREV_SONG            = 0x02; // Command to return to the previous song
  static const byte CMD_SPECIFY_TRACK        = 0x03; // Set the track number - 0 .. 2999
  static const byte CMD_VOLUME_UP            = 0x04; // Increase volume
  static const byte CMD_VOLUME_DOWN          = 0x05; // Decrease volume
  static const byte CMD_SET_VOLUME           = 0x06; // Set volume - 0-30
  static const byte CMD_SET_EQ               = 0x07; // Set equalizer - 0=Normal / 1=Pop / 2=Rock / 3=Jazz / 4=Classic / 5=Bass
  static const byte CMD_SET_PLAYBACK_MODE    = 0x08; // Enable repeat playback - 0=file / 1=folder / 2=single / 3=random
  static const byte CMD_PLAYBACK_SOURCE      = 0x09; // Set playback mode - 0=U / 1=TF / 2=AUX / 3=Sleep / 4=Flash
  static const byte CMD_GO_STANDBY           = 0x0A; // Go to sleep, save energy
  static const byte CMD_NORMAL_OPERATE       = 0x0B; // Normal working?
  static const byte CMD_RESET                = 0x0C; // Reset module
  static const byte CMD_RESUME               = 0x0D; // Start playing
  static const byte CMD_PAUSE_PLAY           = 0x0E; // Pause playback
  static const byte CMD_SET_PLAY_FOLDER_FILE = 0x0F; // Set folder to play - 1-10 (must be set by user)
  static const byte CMD_VOLUME_ADJUST        = 0x10; // DH=0x01 - open volume adjust, DL=set volume gain 0-31 decimal
  static const byte CMD_REPEAT_PLAY          = 0x11; // 0=Stop repeat play / 1=Start repeat play
  static const byte CMD_PLAY_ADVERT          = 0x13; // Play advertisement
  static const byte CMD_STOP_ADVERT          = 0x15; // Stop advertisement
  static const byte CMD_STOP                 = 0x16; // Stop playback

  static const byte CMD_MEDIUM_INSERTED      = 0x3A; // Medium inserted - 1=USB, 2=SD, 3=USB+SD
  static const byte CMD_MEDIUM_REMOVED       = 0x3B; // Medium removed - 1=USB, 2=SD, 3=USB+SD
  static const byte CMD_PLAYBACk_FINISHED1   = 0x3C; // Playback finished
  static const byte CMD_PLAYBACk_FINISHED2   = 0x3D; // Playback finished
  static const byte CMD_UNKNOWN1             = 0x3E; // ?
  static const byte CMD_INIT                 = 0x3F; // Send init params 0x0F (each bit represents one device - lower 4 bits)

  static const byte CMD_ERROR                = 0x40; // Returns error, requests repetition
  static const byte CMD_ACK                  = 0x41; // ACK
  static const byte CMD_STATUS               = 0x42; // Query current status
  static const byte CMD_VOLUME               = 0x43; // Query current volume
  static const byte CMD_EQ                   = 0x44; // Query current equalizer
  static const byte CMD_PLAYBACK_MODE        = 0x45; // Query current playback mode
  static const byte CMD_SOFTWARE_VERSION     = 0x46; // Query current software version
  static const byte CMD_TF_CARD_FILES        = 0x47; // Query total number of files on TF card
  static const byte CMD_UDISK_FILES          = 0x48; // Query total number of files on U disk
  static const byte CMD_FLASH_FILES          = 0x49; // Query total number of files in flash memory
  static const byte CMD_KEEP_ON              = 0x4A; // Keep on
  static const byte CMD_TF_CARD_TRACK        = 0x4B; // Query current track on TF card
  static const byte CMD_UDISK_TRACK          = 0x4C; // Query current track on U disk
  static const byte CMD_FLASH_TRACK          = 0x4D; // Query current track in flash memory
  static const byte CMD_UNKNOWN2             = 0x4E; // ?
  static const byte CMD_UNKNOWN3             = 0x4F; // ?

  static const byte Start_Byte     = 0x7E;
  static const byte Version_Byte   = 0xFF;
  static const byte Command_Length = 0x06;
  static const byte End_Byte       = 0xEF;
  static const byte Acknowledge    = 0x00;

  static const byte DEFAULT_DIRECTORY_NUMBER = 1;

  static const int DFPLAYER_RESET_TIME = 1000;
  static const int DFPLAYER_INIT_TIME = 500;

public:
  static const bool ACK_ENABLED  = true;
  static const bool ACK_DISABLED = false;

private:
  #define DFPLAYER_RECEIVED_LENGTH 10
  #define DFPLAYER_SEND_LENGTH 10

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

  #define EqNormal 0
  #define EqPop 1
  #define EqRock 2
  #define EqJazz 3
  #define EqClassic 4
  #define EqBase 5

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
  SoftwareSerial mySerial;  // SoftwareSerial object for communication with DFPlayer Mini

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
  int eqset;
  uint32_t lastSubmitMillis; // Global variable to track the last command submission time
  boolean playing = false;

private:
  volatile int volume = 20;

public:
  DFPlay();
  void setup(byte newVolume, bool enableAck);
  void update();
  void reset();
  void init();
  bool getPlaying();
  byte getVolume();
  void setVolume(byte newVolume);
  void volumeIncrement();
  void volumeDecrement();
  void play(byte directoryNumber, byte fileNumber);
  void play(byte fileNumber);
  void pause();
  void stop();
  void setEqualizer(byte equalizerType);
  void playAdvertise(int track);
  void stopAdvertise();

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
  void cycleEqualizer();
  void exe_cmd(byte CMD, byte Par1, byte Par2);
};

