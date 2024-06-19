#include "DFPlay.h"

// Constructor initializes the SoftwareSerial with the specified pins
DFPlay::DFPlay()
  : dfPlayerSerial(PIN_MP3_RX, PIN_MP3_TX) {
}

// Setup initializes the DFPlayer with the specified volume and acknowledgment setting
void DFPlay::setup(byte newVolume, bool enableAck) {
  playing = false;

  dfPlayerSerial.begin(9600);
  if (enableAck) {
    enableACK();
  } else {
    disableACK();
  }

  //reset(true); // At boot time the reset is not necessary.
  init(false); // false parameter means: no not wait

  setVolume(newVolume);
  waitAndEatAvailable(DFPLAYER_INIT_TIME);

#ifdef _DEBUG
  Serial.println("DFPlayer ready");
#endif
}

// Update the status of the DFPlayer
void DFPlay::update() {
  available();
}

// Reset the DFPlayer module
void DFPlay::reset(bool performWaiting) {
  exe_cmd(CMD_RESET, 0, 0);
  if (performWaiting) {
    waitAndEatAvailable(DFPLAYER_RESET_TIME); // Wait for the DFPlayer to restart
  }
}

// Initialize the DFPlayer module
void DFPlay::init(bool performWaiting) {
  exe_cmd(CMD_INIT, 0, DFPLAYER_DEVICE_SD);
  if (performWaiting) {
    waitAndEatAvailable(DFPLAYER_INIT_TIME); // Wait for the DFPlayer to initialize
  }
}

// Play a specific file in a specific folder
void DFPlay::play(byte directoryNumber, byte fileNumber) {
  exe_cmd(CMD_SET_PLAY_FOLDER_FILE, directoryNumber, fileNumber);
  playing = true;
}

// Play a specific file in the default folder
void DFPlay::play(byte fileNumber) {
  play(DEFAULT_DIRECTORY_NUMBER, fileNumber);
}

// Stop the playback
void DFPlay::stop() {
  exe_cmd(CMD_STOP, 0, 1);
  playing = false;
}

// Pause the playback
void DFPlay::pause() {
#ifdef _DEBUG
  Serial.print(F("Is playing: "));
  Serial.print(playing);
#endif
  if (playing) {
    exe_cmd(CMD_PAUSE_PLAY, 0, 0);
    playing = false;
  }
}

// Check if the player is currently playing
bool DFPlay::getPlaying() {
  return playing;
}

// Get the current volume
byte DFPlay::getVolume() {
  return (byte)(volume & 0xFF);
}

// Set a new volume
void DFPlay::setVolume(byte newVolume) {
  volume = newVolume;
  exe_cmd(CMD_SET_VOLUME, 0, volume);
}

// Increase the volume by one step
void DFPlay::volumeIncrement() {
  volume++;
  if (volume > MAX_VOLUME) {
    volume = MAX_VOLUME;
  }
  exe_cmd(CMD_SET_VOLUME, 0, volume);
}

// Decrease the volume by one step
void DFPlay::volumeDecrement() {
  volume--;
  if (volume < 0) {
    volume = 0;
  }
  exe_cmd(CMD_SET_VOLUME, 0, volume);
}

// Cycle through equalizer settings
void DFPlay::cycleEqualizer() {
  eqset = eqset + 1;
  if (eqset == EqBase + 1) {
    eqset = EqNormal;
  }

  setEqualizer(eqset);
}

// Set the equalizer to a specific type
void DFPlay::setEqualizer(byte equalizerType) {
  exe_cmd(CMD_SET_EQ, 0, equalizerType);
}

// Play an advertisement track
void DFPlay::playAdvertise(int track) {
  exe_cmd(CMD_PLAY_ADVERT, 0, track);
}

// Stop the advertisement
void DFPlay::stopAdvertise() {
  exe_cmd(CMD_STOP_ADVERT, 0, 0);
}

// Execute a command with parameters
void DFPlay::exe_cmd(byte CMD, byte Par1, byte Par2) {
  available();
  uint32_t submitGap = millis() - lastSubmitMillis;

  // Ensure a minimum gap of 30ms between commands
  if (submitGap < 30) {
    delay(30 - submitGap);
  }

  lastSubmitMillis = millis();

  _sending[Stack_Command] = CMD;
  _sending[Stack_Parameter] = Par1;
  _sending[Stack_Parameter + 1] = Par2;

  uint16_t checksum = calculateCheckSum(_sending);

  _sending[Stack_CheckSum] = (byte)(checksum >> 8);        // High byte of checksum
  _sending[Stack_CheckSum + 1] = (byte)(checksum & 0xFF);  // Low byte of checksum

  sendStack();
}

// Handle received messages
bool DFPlay::handleMessage(uint8_t type, uint16_t parameter) {
  _receivedIndex = 0;
  _handleType = type;
  _handleParameter = parameter;
  _isAvailable = true;

  return _isAvailable;
}

// Handle errors and reset sending flag
bool DFPlay::handleError(uint8_t type, uint16_t parameter) {
  handleMessage(type, parameter);
  _isSending = false;

#ifdef _DEBUG
  Serial.println("Error");
#endif

  return false;
}

// Wait for available data within a specified duration
bool DFPlay::waitAvailable(unsigned long duration) {
  unsigned long timer = millis();
  if (!duration) {
    duration = _timeOutDuration;
  }
  while (!available()) {
    if (millis() - timer > duration) {
      return handleError(TimeOut);
    }

    delay(0);
  }

  return true;
}

// Wait for available data within a specified duration
void DFPlay::waitAndEatAvailable(unsigned long duration) {
  unsigned long timeOut = millis() + duration;
  while (timeOut > millis() || dfPlayerSerial.available()) {
    if (dfPlayerSerial.available()) {
      dfPlayerSerial.read();
      continue;
    }
    delay(2);
  }
}

// Convert a byte array to uint16_t
uint16_t DFPlay::arrayToUint16(uint8_t *array) {
  uint16_t value = *array;
  value <<= 8;
  value += *(array + 1);
  return value;
}

// Validate the received stack
bool DFPlay::validateStack() {
  return calculateCheckSum(_received) == arrayToUint16(_received + Stack_CheckSum);
}

// Parse the received stack
void DFPlay::parseStack() {
  uint8_t handleCommand = *(_received + Stack_Command);
  if (handleCommand == CMD_ACK) {
#ifdef _DEBUG
    Serial.println("ACK feedback");
#endif
    _isSending = false;
    return;
  }

  _handleCommand = handleCommand;
  _handleParameter = arrayToUint16(_received + Stack_Parameter);

#ifdef _DEBUG
  printDetail(handleCommand, _received[Stack_Parameter1], _received[Stack_Parameter2]);
#endif

  switch (_handleCommand) {
    case CMD_PLAYBACk_FINISHED1:
    case CMD_PLAYBACk_FINISHED2:
      playing = false;
      handleMessage(DFPlayerPlayFinished, _handleParameter);
      break;
    case CMD_INIT:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBOnline, _handleParameter);
      } else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardOnline, _handleParameter);
      } else if (_handleParameter & 0x03) {
        handleMessage(DFPlayerCardUSBOnline, _handleParameter);
      }
      break;
    case CMD_MEDIUM_INSERTED:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBInserted, _handleParameter);
      } else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardInserted, _handleParameter);
      }
      break;
    case CMD_MEDIUM_REMOVED:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBRemoved, _handleParameter);
      } else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardRemoved, _handleParameter);
      }
      break;
    case CMD_ERROR:
      handleMessage(DFPlayerError, _handleParameter);
      break;
    case CMD_UNKNOWN1:
    case CMD_STATUS:
    case CMD_VOLUME:
    case CMD_EQ:
    case CMD_PLAYBACK_MODE:
    case CMD_SOFTWARE_VERSION:
    case CMD_TF_CARD_FILES:
    case CMD_UDISK_FILES:
    case CMD_FLASH_FILES:
    case CMD_KEEP_ON:
    case CMD_TF_CARD_TRACK:
    case CMD_UDISK_TRACK:
    case CMD_FLASH_TRACK:
    case CMD_UNKNOWN2:
    case CMD_UNKNOWN3:
      handleMessage(DFPlayerFeedBack, _handleParameter);
      break;
    default:
      handleError(WrongStack);
      break;
  }
}

// Enable acknowledgment
void DFPlay::enableACK() {
  _sending[Stack_ACK] = 0x01;
}

// Disable acknowledgment
void DFPlay::disableACK() {
  _sending[Stack_ACK] = 0x00;
}

// Calculate the checksum for the command stack
uint16_t DFPlay::calculateCheckSum(byte *buffer) {
  uint16_t sum = 0;
  for (int i = 1; i < 7; i++) {  // Start from buffer[1] (Version_Byte) to buffer[6] (Par2)
    sum += buffer[i];
  }
  return -sum;
}

// Check if data is available and read it
bool DFPlay::available() {
  while (dfPlayerSerial.available()) {
    delay(0);
    if (_receivedIndex == 0) {
      _received[Stack_Header] = dfPlayerSerial.read();
#ifdef _DEBUG
      Serial.print(F("received: "));
      Serial.print(_received[_receivedIndex], HEX);
      Serial.print(F(" "));
#endif
      if (_received[Stack_Header] == 0x7E) {
        _receivedIndex++;
      }
    } else {
      _received[_receivedIndex] = dfPlayerSerial.read();
#ifdef _DEBUG
      Serial.print(_received[_receivedIndex], HEX);
      Serial.print(F(" "));
#endif
      switch (_receivedIndex) {
        case Stack_Version:
          if (_received[_receivedIndex] != 0xFF) {
            return handleError(WrongStack);
          }
          break;
        case Stack_Length:
          if (_received[_receivedIndex] != 0x06) {
            return handleError(WrongStack);
          }
          break;
        case Stack_End:
#ifdef _DEBUG
          Serial.println();
#endif
          if (_received[_receivedIndex] != 0xEF) {
            return handleError(WrongStack);
          } else {
            if (validateStack()) {
              _receivedIndex = 0;
              parseStack();
              return _isAvailable;
            } else {
              return handleError(WrongStack);
            }
          }
          break;
        default:
          break;
      }
      _receivedIndex++;
    }
  }

  return _isAvailable;
}

// Send the command stack to DFPlayer
void DFPlay::sendStack() {
  if (_sending[Stack_ACK]) {  // If ACK mode is on, wait until the last transmission
    while (_isSending) {
      delay(0);
      waitAvailable();
    }
  }

#ifdef _DEBUG
  Serial.println();
  Serial.print(F("sending:"));
  for (int i = 0; i < DFPLAYER_SEND_LENGTH; i++) {
    Serial.print(_sending[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();
#endif

  dfPlayerSerial.write(_sending, DFPLAYER_SEND_LENGTH);
  _timeOutTimer = millis();
  _isSending = _sending[Stack_ACK];

  if (!_sending[Stack_ACK]) {  // If ACK mode is off, wait 10 ms after one transmission.
    delay(10);
  }

#ifdef _DEBUG
  printDetail(_sending[Stack_Command], _sending[Stack_Parameter1], _sending[Stack_Parameter2]);
  Serial.println("Sent.");
  Serial.println();
#endif
}

// Print detailed information for debugging
void DFPlay::printDetail(uint8_t cmd, uint8_t param1, uint8_t param2) {
#ifndef _DEBUG
  return;
#endif

  uint16_t params = (param1 << 8) + param2;
  switch (cmd) {
    case CMD_NEXT_SONG:
      Serial.println(F("CMD_NEXT_SONG"));
      break;
    case CMD_PREV_SONG:
      Serial.println(F("CMD_PREV_SONG"));
      break;
    case CMD_SPECIFY_TRACK:
      Serial.print(F("CMD_SPECIFY_TRACK "));
      Serial.println(params, HEX);
      break;
    case CMD_VOLUME_UP:
      Serial.println(F("CMD_VOLUME_UP"));
      break;
    case CMD_VOLUME_DOWN:
      Serial.println(F("CMD_VOLUME_DOWN"));
      break;
    case CMD_SET_VOLUME:
      Serial.print("CMD_SET_VOLUME ");
      Serial.println(params);
      break;
    case CMD_SET_EQ:
      Serial.print("CMD_SET_EQ ");
      Serial.println(params, HEX);
      break;
    case CMD_SET_PLAYBACK_MODE:
      Serial.print(F("CMD_SET_PLAYBACK_MODE "));
      Serial.println(params, HEX);
      break;
    case CMD_PLAYBACK_SOURCE:
      Serial.print(F("CMD_PLAYBACK_SOURCE "));
      Serial.println(params, HEX);
      break;
    case CMD_GO_STANDBY:
      Serial.println(F("CMD_GO_STANDBY"));
      break;
    case CMD_NORMAL_OPERATE:
      Serial.println(F("CMD_NORMAL_OPERATE"));
      break;
    case CMD_RESET:
      Serial.println(F("CMD_RESET"));
      break;
    case CMD_RESUME:
      Serial.println(F("CMD_RESUME"));
      break;
    case CMD_PAUSE_PLAY:
      Serial.println(F("CMD_PAUSE_PLAY"));
      break;
    case CMD_SET_PLAY_FOLDER_FILE:
      Serial.print(F("CMD_SET_PLAY_FOLDER_FILE folder: "));
      Serial.print(param1);
      Serial.print(F(", file: "));
      Serial.println(param2);
      break;
    case CMD_VOLUME_ADJUST:
      Serial.print(F("CMD_VOLUME_ADJUST "));
      Serial.println(params, HEX);
      break;
    case CMD_REPEAT_PLAY:
      Serial.print(F("CMD_REPEAT_PLAY "));
      Serial.println(params, HEX);
      break;
    case CMD_MEDIUM_INSERTED:
      Serial.print(F("Medium inserted "));
      Serial.println(params, HEX);
      break;
    case CMD_MEDIUM_REMOVED:
      Serial.print(F("Medium removed "));
      Serial.println(params, HEX);
      break;
    case CMD_PLAYBACk_FINISHED1:
    case CMD_PLAYBACk_FINISHED2:
      Serial.print(F("Playback finished, file: "));
      Serial.println(params);
      break;
    case CMD_INIT:
      Serial.print(F("CMD_INIT: "));
      Serial.println(params);
      break;
    case CMD_PLAY_ADVERT:
      Serial.print(F("CMD_PLAY_ADVERT: "));
      Serial.println(params);
      break;
    case CMD_STOP_ADVERT:
      Serial.println(F("CMD_STOP_ADVERT: "));
      break;
    case CMD_ERROR:
      Serial.print(F("Error: "));
      switch (params) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("Advertise"));
          break;
        default:
          Serial.print(F("Unknown parameter: "));
          Serial.println(params, HEX);
          break;
      }
      break;
    default:
      Serial.print(F("Unknown command: "));
      Serial.print(cmd, HEX);
      Serial.print(F(", params: "));
      Serial.println(params, HEX);
      break;
  }
}
