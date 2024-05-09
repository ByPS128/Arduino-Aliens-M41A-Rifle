#include "M41aPlayer.h"

M41aPlayer::M41aPlayer() : mySerial(PIN_MP3_RX, PIN_MP3_TX)
{
}

void M41aPlayer::setup(byte newVolume)
{
  Playing = false;

  mySerial.begin(9600);
  delay(500);

  //enableACK();
  disableACK();

  exe_cmd(CMD_INIT, 0, 0x02);

  volume = newVolume;
  exe_cmd(CMD_SET_VOLUME, 0, volume);

  #ifdef _DEBUG
  Serial.println("Player ready");
  #endif
}

void M41aPlayer::update()
{
  available();
}

void M41aPlayer::stop()
{
  exe_cmd(0x16,0,1);
}

void M41aPlayer::pause()
{
  #ifdef _DEBUG
  Serial.print(F("Is playing: "));
  Serial.print(Playing);
  #endif
  if (Playing) {
    exe_cmd(0x0E,0,0);
    Playing = false;
  }
}

byte M41aPlayer::getVolume()
{
  return (byte)(volume & 0xFF);
}

void M41aPlayer::setVolume(byte newVolume)
{
  volume = newVolume;
  exe_cmd(0x06, 0, volume);
}

void M41aPlayer::volumeINC()
{
  volume++;
  if(volume > MAX_VOLUME)
  {
    volume = MAX_VOLUME;
  }
  exe_cmd(0x06, 0, volume);
  playClick();
}

void M41aPlayer::volumeDEC()
{
  volume--;
  if(volume < 0)
  {
    volume = 0;
  }
  exe_cmd(0x06, 0, volume);
  playClick();
}

void M41aPlayer::equalizer()
{
  eqset = eqset+1;
  if(eqset == 6)
  {
    eqset = 0;
  }
  exe_cmd(0x07, 0 ,eqset);
}

void M41aPlayer::playFire(int bulletsCount)
{
  if (bulletsCount > 0)
  {
    int shootFile = lastFireInstrument;
    while (lastFireInstrument == shootFile)
    {
      shootFile = random(SAMPLE_FIRE_FIRST, SAMPLE_FIRE_LAST + 1);
    }

    exe_cmd(0x0F, 1, shootFile);
  } else {
    playEmptyMagazine();
  }

  Playing = true;
}

void M41aPlayer::playReload()
{
  int reloadFile = random(SAMPLE_COCK_FIRST, SAMPLE_COCK_LAST + 1);
  exe_cmd(0x0F, 1, reloadFile);
  Playing = true;
}

void M41aPlayer::playEmptyMagazine()
{
  int emptyFile = lastEmptyMagazineInstrument;
  while (lastEmptyMagazineInstrument == emptyFile)
  {
    emptyFile = random(SAMPLE_EMPTY_MAGAZINE_FIRST, SAMPLE_EMPTY_MAGAZINE_LAST + 1);
  }

  exe_cmd(0x0F, 1, emptyFile);
  Playing = true;
}

void M41aPlayer::playClick()
{
  return;

  if (Playing) {
    return;
  }

  exe_cmd(0x0F, 1, SAMPLE_CLICK);
  Playing = true;
}

void M41aPlayer::playVolumeSaved()
{
  exe_cmd(0x0F, 1, SAMPLE_VOLUME_SAVED);
  Playing = true;
}

void M41aPlayer::playVolumeUp()
{
  exe_cmd(0x0F, 1, SAMPLE_VOLUME_UP);
  Playing = true;
}

void M41aPlayer::playVolumeDown()
{
  exe_cmd(0x0F, 1, SAMPLE_VOLUME_DOWN);
  Playing = true;
}

void M41aPlayer::playCardInserted()
{
  exe_cmd(0x0F, 1, SAMPLE_CARD_INSERTED);
  Playing = true;
}

void M41aPlayer::playCardRemoved()
{
  exe_cmd(0x0F, 1, SAMPLE_CARD_REMOVED);
  Playing = true;
}

void M41aPlayer::playLowBattery()
{
  exe_cmd(0x0F, 1, SAMPLE_LOW_BATTERY);
  Playing = true;
}

void M41aPlayer::playWeaponReady()
{
  exe_cmd(0x0F, 1, SAMPLE_WEAPON_READY);
  Playing = true;
}

void M41aPlayer::exe_cmd(byte CMD, byte Par1, byte Par2)
{
  available();
  // Aktuální čas od posledního odeslání
  uint32_t submitGap = millis() - lastSubmitMillis;
  
  // Pokud je prodleva menší než 30 ms, čekáme, dokud neuplyne 30 ms
  if (submitGap < 30) {
    //delay(30 - submitGap);
  }
  
  // Aktualizace času posledního odeslání příkazu
  lastSubmitMillis = millis();

  // Sestavení příkazové řady
  //byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, 0, 0, End_Byte};
  //_sending[Stack_Length] = Command_Length;
  _sending[Stack_Command] = CMD;
  _sending[Stack_Parameter] = Par1;
  _sending[Stack_Parameter+1] = Par2;

  // Výpočet kontrolního součtu
  uint16_t checksum = calculateCheckSum(_sending);

  // Přidání kontrolního součtu do příkazové řady
  // Command_line[7] = (byte)(checksum >> 8); // Horní bajt checksumu
  // Command_line[8] = (byte)(checksum & 0xFF); // Dolní bajt checksumu
  _sending[Stack_CheckSum] = (byte)(checksum >> 8); // Horní bajt checksumu
  _sending[Stack_CheckSum+1] = (byte)(checksum & 0xFF); // Dolní bajt checksumu

  // Odeslání příkazové řady modulu
  // for (byte k = 0; k < 10; k++) {
  //   Serial.write(Command_line[k]);
  // }
  sendStack();
}

bool M41aPlayer::handleMessage(uint8_t type, uint16_t parameter){
  _receivedIndex = 0;
  _handleType = type;
  _handleParameter = parameter;
  _isAvailable = true;

  return _isAvailable;
}

bool M41aPlayer::handleError(uint8_t type, uint16_t parameter = 0){
  handleMessage(type, parameter);
  _isSending = false;

  #ifdef _DEBUG
  Serial.println("Error");
  #endif

  return false;
}

bool M41aPlayer::waitAvailable(unsigned long duration = 0){
  unsigned long timer = millis();
  if (!duration) {
    duration = _timeOutDuration;
  }
  while (!available()){
    if (millis() - timer > duration) {
      return handleError(TimeOut);
    }
    delay(0);
  }
  return true;
}

uint16_t M41aPlayer::arrayToUint16(uint8_t *array) {
  uint16_t value = *array;
  value <<=8;
  value += *(array+1);
  return value;
}

bool M41aPlayer::validateStack(){
  return calculateCheckSum(_received) == arrayToUint16(_received+Stack_CheckSum);
}

void M41aPlayer::parseStack(){
  uint8_t handleCommand = *(_received + Stack_Command);
  if (handleCommand == 0x41) { //handle the 0x41 ack feedback as a spcecial case, in case the pollusion of _handleCommand, _handleParameter, and _handleType.
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
    case 0x3C:
    case 0x3D:
      Playing = false;
      handleMessage(DFPlayerPlayFinished, _handleParameter);
      // software repat
      //delay(100);
      //exe_cmd(0x0D, 0, 0);
      break;
    case CMD_INIT:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBOnline, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardOnline, _handleParameter);
      }
      else if (_handleParameter & 0x03) {
        handleMessage(DFPlayerCardUSBOnline, _handleParameter);
      }
      break;
    case 0x3A:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBInserted, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardInserted, _handleParameter);
      }
      break;
    case 0x3B:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBRemoved, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardRemoved, _handleParameter);
      }
      break;
    case 0x40:
      handleMessage(DFPlayerError, _handleParameter);
      // signalError(_handleParameter)
      //sendStack();
      break;
    case 0x3E:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
      handleMessage(DFPlayerFeedBack, _handleParameter);
      break;
    default:
      handleError(WrongStack);
      break;
  }
}

void M41aPlayer::enableACK(){
  _sending[Stack_ACK] = 0x01;
}

void M41aPlayer::disableACK(){
  _sending[Stack_ACK] = 0x00;
}

uint16_t M41aPlayer::calculateCheckSum(byte *buffer) {
  uint16_t sum = 0;
  for (int i = 1; i < 7; i++) { // Začínáme od buffer[1] (Version_Byte) a končíme buffer[6] (Par2)
    sum += buffer[i];
  }
  return -sum;
}

bool M41aPlayer::available(){
  while (mySerial.available()) {
    delay(0);
    if (_receivedIndex == 0) {
      _received[Stack_Header] = mySerial.read();
      #ifdef _DEBUG
      Serial.print(F("received: "));
      Serial.print(_received[_receivedIndex],HEX);
      Serial.print(F(" "));
      #endif
      if (_received[Stack_Header] == 0x7E) {
        _receivedIndex ++;
      }
    }
    else{
      _received[_receivedIndex] = mySerial.read();
      #ifdef _DEBUG
      Serial.print(_received[_receivedIndex],HEX);
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
          }
          else{
            if (validateStack()) {
              _receivedIndex = 0;
              parseStack();
              return _isAvailable;
            }
            else{
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

void M41aPlayer::sendStack(){
  if (_sending[Stack_ACK]) {  //if the ack mode is on wait until the last transmition
    while (_isSending) {
      delay(0);
      waitAvailable();
    }
  }

  #ifdef _DEBUG
  Serial.println();
  Serial.print(F("sending:"));
  for (int i=0; i<DFPLAYER_SEND_LENGTH; i++) {
    Serial.print(_sending[i],HEX);
    Serial.print(F(" "));
  }
  Serial.println();
  #endif

  mySerial.write(_sending, DFPLAYER_SEND_LENGTH);
  _timeOutTimer = millis();
  _isSending = _sending[Stack_ACK];
  
  if (!_sending[Stack_ACK]) { //if the ack mode is off wait 10 ms after one transmition.
    delay(10);
  }

  #ifdef _DEBUG
  printDetail(_sending[Stack_Command], _sending[Stack_Parameter1], _sending[Stack_Parameter2]);
  Serial.println("Sent.");
  Serial.println();
  #endif
}

void M41aPlayer::printDetail(uint8_t cmd, uint8_t param1, uint8_t param2) {
    #ifndef _DEBUG
    return;
    #endif;

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
    case CMD_RESET_MODULE:
      Serial.println(F("CMD_RESET_MODULE"));
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


    case 0x3A:
      Serial.print(F("Mnedium inserted "));
      Serial.println(params, HEX);
      break;
    case 0x3B:
      Serial.print(F("Medium removed "));
      Serial.println(params, HEX);
      break;
    case 0x3C:
    case 0x3D:
      Serial.print(F("Playback finished, file: "));
      Serial.println(params);
      break;
    case CMD_INIT:
      Serial.print(F("CMD_INIT: "));
      Serial.println(params);
      break;
    case 0x40:
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
          Serial.println(F("In Advertise"));
          break;
        default:
          // value
          Serial.print(F("Unknown parameter: "));
          Serial.println(params, HEX);
          break;
      }
      break;
    default:
      // type
      Serial.print(F("Unknown command: "));
      Serial.print(cmd, HEX);
      Serial.print(F(", params: "));
      Serial.println(params, HEX);
      break;
  }
}
