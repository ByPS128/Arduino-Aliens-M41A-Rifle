#include <SoftwareSerial.h>

/*

forma:

Start Byte   -- 0x7E
Version Byte -- 0xFF
Length Byte
Command Byte
SetFeedback Byte (01/00 = yes/no)
Parameter1 Byte Hi Byte of Word
Parameter2 Byte Li Byte of Word
Checksum 2 Bytes
End Byte     -- 0xEF


| Start | Version | Length | Command | SetFeedback | Param #1 | Param #2 | ChkSum #1 | ChkSum #2 | End  |
| 0x7E  | 0xFF    | ..     | ..      | 0x00 / 0x01 | ..       | ..       | ..        | ..        | 0xEF |

*/

// Commands :
static const byte CMD_NEXT_SONG          = 0x01; // Příkaz pro přeskočení na další skladbu
static const byte CMD_PREV_SONG          = 0x02; // Příkaz pro návrat na předchozí skladbu
static const byte CMD_SPECIFY_TRACK      = 0x03; // Nastavení čísla skldaby - 0 .. 2999
static const byte CMD_VOLUME_UP          = 0x04; // Zvýšení hlasitosti
static const byte CMD_VOLUME_DOWN        = 0x05; // Snížení hlasitosti
static const byte CMD_SET_VOLUME         = 0x06; // Nastavení hlasitosti - 0-30
static const byte CMD_SET_EQ             = 0x07; // Nastavení ekvalizéru - 0=Normal / 1=Pop / 2=Rock / 3=Jazz / 4=Classic / 5=Base
static const byte CMD_SET_PLAYBACK_MODE  = 0x08; // Zapnutí opakování přehrání - 0=file / 1=folder / 2=single / 3=random
static const byte CMD_PLAYBACK_SOURCE    = 0x09; // Nastavení režimu přehrávání - 0=U / 1=TF / 2=AUX / 3=Sleep / 4=Flash
static const byte CMD_GO_STANDBY         = 0x0A; // Jdi spát, šetři energii
static const byte CMD_NORMAL_OPERATE     = 0x0B; // Normal working ?
static const byte CMD_RESET_MODULE       = 0x0C; // Resetovat modul
static const byte CMD_RESUME             = 0x0D; // Začít přehrávat
static const byte CMD_PAUSE_PLAY         = 0x0E; // Pozastavit přehrávání
static const byte CMD_SET_FOLDER_TO_PLAY = 0x0F; // Nastavit složku k přehrávání - 1-10 (nutno nastavit uživatelem)
static const byte CMD_VOLUME_ADJUST      = 0x10; // DH=0x01 - open volume adjust, DL=set volume gain 0-31 decimal
static const byte CMD_REPEAT_PLAY        = 0x11; // 0=Stop repeat play / 1=Start repeat play

static const byte CMD_STAY1              = 0x3C; // ?
static const byte CMD_STAY2              = 0x3D; // ?
static const byte CMD_STAY3              = 0x3E; // ?
static const byte CMD_INIT               = 0x3F; // Send init params 0x0F (každý bit reprezentuje jedno zařízení - spodní 4 bity)

static const byte CMD_ERROR              = 0x40; // Vrátí chybu, vyžádá opakování
static const byte CMD_REPLY              = 0x41; // Odpověď
static const byte CMD_STATUS             = 0x42; // Dotaz na aktuální stav
static const byte CMD_VOLUME             = 0x43; // Dotaz na aktuální hlasitost
static const byte CMD_EQ                 = 0x44; // Dotaz na aktuální ekvalizér
static const byte CMD_PLAYBACK_MODE      = 0x45; // Dotaz na aktuální režim přehrávání
static const byte CMD_SOFTWARE_VERSION   = 0x46; // Dotaz na aktuální verzi software
static const byte CMD_TF_CARD_FILES      = 0x47; // Dotaz na celkový počet souborů na TF kartě
static const byte CMD_UDISK_FILES        = 0x48; // Dotaz na celkový počet souborů na U disku
static const byte CMD_FLASH_FILES        = 0x49; // Dotaz na celkový počet souborů ve flash paměti
static const byte CMD_KEEP_ON            = 0x4A; // Ponechat zapnuté
static const byte CMD_TF_CARD_TRACK      = 0x4B; // Dotaz na aktuální skladbu na TF kartě
static const byte CMD_UDISK_TRACK        = 0x4C; // Dotaz na aktuální skladbu na U disku
static const byte CMD_FLASH_TRACK        = 0x4D; // Dotaz na aktuální skladbu ve flash paměti

static const byte Start_Byte     = 0x7E;
static const byte Version_Byte   = 0xFF;
static const byte Command_Length = 0x06;
static const byte End_Byte       = 0xEF;
static const byte Acknowledge    = 0x00;

# define ACTIVATED LOW

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX
const int button1_Pin = 4; // spošť puška
const int button2_Pin = 5; // přebít
const int button3_Pin = 6; // granátomet tam
const int button4_Pin = 7; // granátomet zpět
const int button5_Pin = 8; // zásobník výměna 

const int rotaryS1_Pin = 9; // volume encoder
const int rotaryS2_Pin = 10; // volume encoder

const int counter1_Pin = 11; // 7segment counter 
const int counter2_Pin = 12; 
const int counter3_Pin = 13; 

const int led_Pin = A0; 

int lastButton1_State = HIGH;
int lastButton2_State = HIGH;

// Proměnné pro uchování posledního stavu
bool usedRcFilter = true;
bool useSoftVolumeRotaryEncoder = false;
int oldRotaryMasterValue = -1;
volatile unsigned long lastVolumeEncoderRun = 0;
volatile int newVolume = 0;
bool ledState = false;
int debounceTreshold = 100;
int rotaryMasterValue = 0;

SoftwareSerial mySerial(PIN_MP3_RX, PIN_MP3_TX);

// Globální proměnná pro sledování času posledního odeslání příkazu
uint32_t lastSubmitMillis = 0;

volatile int volume = 15;
int eqset = 0;
boolean Playing = false;


#define _DEBUG 1


#define DFPLAYER_RECEIVED_LENGTH 10
#define DFPLAYER_SEND_LENGTH 10

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

#define Stack_Header 0
#define Stack_Version 1
#define Stack_Length 2
#define Stack_Command 3
#define Stack_ACK 4
#define Stack_Parameter 5
#define Stack_CheckSum 7
#define Stack_End 9

#define DFPLAYER_DEVICE_U_DISK 1
#define DFPLAYER_DEVICE_SD 2
#define DFPLAYER_DEVICE_AUX 3
#define DFPLAYER_DEVICE_SLEEP 4
#define DFPLAYER_DEVICE_FLASH 5


  unsigned long _timeOutDuration = 500;
  unsigned long _timeOutTimer;
  uint8_t _received[DFPLAYER_RECEIVED_LENGTH];
  uint8_t _sending[DFPLAYER_SEND_LENGTH] = {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
  uint8_t _receivedIndex=0;
  uint8_t _handleType;
  uint8_t _handleCommand;
  uint16_t _handleParameter;
  bool _isAvailable = false;
  bool _isSending = false;





// Define Connections to 74HC595
int latchPin=12;  //74HC595 RCLK  pin 12 ST_CP - zelený
int clockPin=11;  //74HC595 SRCLK pin 11 SH_CP - žlutý
int dataPin=13;   //74HC595 SER   pin 14 DS    - modrý







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

void printDetail(){
    switch (_handleCommand) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(_handleParameter, HEX);
      Serial.println(F(" Play Finished!"));
      break;
    case 0x3B:
      Serial.println(F("SD card removed."));
    case 0x3C:
    case 0x3D:
      Serial.println(F("Plyback finished."));
      break;
    case 0x3F:
      Serial.println(F("SD card inserted."));
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (_handleParameter) {
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
          Serial.println(_handleParameter, HEX);
          break;
      }
      break;
    default:
      // type
      Serial.print(F("Unknown command: "));
      Serial.println(_handleCommand, HEX);
      break;
  }
}




bool handleMessage(uint8_t type, uint16_t parameter){
  _receivedIndex = 0;
  _handleType = type;
  _handleParameter = parameter;
  _isAvailable = true;

  return _isAvailable;
}

bool handleError(uint8_t type, uint16_t parameter = 0){
  handleMessage(type, parameter);
  _isSending = false;

  return false;
}

bool waitAvailable(unsigned long duration = 0){
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

uint16_t arrayToUint16(uint8_t *array){
  uint16_t value = *array;
  value <<=8;
  value += *(array+1);
  return value;
}

bool validateStack(){
  return calculateCheckSum(_received) == arrayToUint16(_received+Stack_CheckSum);
}

void parseStack(){
  uint8_t handleCommand = *(_received + Stack_Command);
  if (handleCommand == 0x41) { //handle the 0x41 ack feedback as a spcecial case, in case the pollusion of _handleCommand, _handleParameter, and _handleType.
    Serial.println("ACK feedback");
    _isSending = false;
    return;
  }
  
  _handleCommand = handleCommand;
  _handleParameter =  arrayToUint16(_received + Stack_Parameter);

  printDetail();

  switch (_handleCommand) {
    case 0x3C:
    case 0x3D:
      handleMessage(DFPlayerPlayFinished, _handleParameter);
      // software repat
      //delay(100);
      //exe_cmd(0x0D, 0, 0);
      break;
    case 0x3F:
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

bool available(){
  while (mySerial.available()) {
    delay(0);
    if (_receivedIndex == 0) {
      _received[Stack_Header] = mySerial.read();
#ifdef _DEBUG
      Serial.print(F("received:"));
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

void sendStack(){
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

  Serial.println("Sent.");
}


void enableACK(){
  _sending[Stack_ACK] = 0x01;
}

void disableACK(){
  _sending[Stack_ACK] = 0x00;
}

uint16_t calculateCheckSum(byte *buffer) {
  uint16_t sum = 0;
  for (int i = 1; i < 7; i++) { // Začínáme od buffer[1] (Version_Byte) a končíme buffer[6] (Par2)
    sum += buffer[i];
  }
  return -sum;
}

void doInterruptVolumeRotaryEncoder()
{
  //if (!usedRcFilter && millis() - lastVolumeEncoderRun <= debounceTreshold) return;

  lastVolumeEncoderRun = millis();
  rotaryMasterValue = digitalRead(rotaryS1_Pin);

  noInterrupts();
  doInternalVolumeDecisions();
  interrupts();
}

void doInternalVolumeDecisions()
{
  if (rotaryMasterValue == 0) return;

  int rotaryValue = digitalRead(rotaryS2_Pin);
  if (rotaryValue == 1) {
    //volumeINC();
    newVolume = volume + 1;
  } else if (rotaryValue == 0) {
    //volumeDEC();
    newVolume = volume - 1;
  }
}
















void setup ()
{
  Serial.begin(9600);
  Serial.println("OK");

  randomSeed(millis());

  pinMode(button1_Pin, INPUT_PULLUP);
  digitalWrite(button1_Pin, HIGH);

  pinMode(button2_Pin, INPUT_PULLUP);
  digitalWrite(button2_Pin, HIGH);

  pinMode(led_Pin, OUTPUT);

  useSoftVolumeRotaryEncoder = (digitalPinToInterrupt(rotaryS1_Pin) != rotaryS1_Pin);
  if (!useSoftVolumeRotaryEncoder) {
    attachInterrupt(digitalPinToInterrupt(rotaryS1_Pin), doInterruptVolumeRotaryEncoder, FALLING);
  }

  pinMode(rotaryS2_Pin, INPUT);

  // Setup display pins as Outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);


  // DF PLayer setup
  mySerial.begin(9600);
  delay(500);
  initMe();
  Playing = false;

  Serial.println("Ready..");
}



int bulletsCount = 95;
int lastNumberToDisplay = -1;
int displayTime = 0;
std::function<void()> afterPlaybackDelegate = null;

void loop ()
{
  doButton1();
  doButton2();

  //delay(50);
  available();

  processVolume();

  displayNumber(bulletsCount);
}






// -------------------------------------------------------------------------------------------------------

void doButton1()
{
  int button1_State = digitalRead(button1_Pin);
  if (button1_State != lastButton1_State) {  // Kontrola, zda došlo ke změně stavu
    if (button1_State == LOW) {
      Serial.println("Button 1 pressed!");
      fireButtonPressed();  // Zavolání funkce při stisku tlačítka
    } else {
      Serial.println("Button 1 released!");
      pause();
    }
    // Aktualizace posledního stavu tlačítka
    lastButton1_State = button1_State;
  }
}

void fireButtonPressed()
{
  if (bulletsCount > 0) {
    int shootFile = random(1, 6+1);
    exe_cmd(0x0F, 1, shootFile);
  } else {
    int emptyMagazineFile = random(10, 14+1);
    exe_cmd(0x0F, 1, emptyMagazineFile);
  }

  //exe_cmd(0x0D, 0, 0);
  Playing = true;
}

void doButton2()
{
  int button2_State = digitalRead(button2_Pin);
  if (button2_State != lastButton2_State) {  // Kontrola, zda došlo ke změně stavu
    if (button2_State == LOW) {
      Serial.println("Button 2 pressed!");
      reloadButtonPressed();  // Zavolání funkce při stisku tlačítka
    } else {
      Serial.println("Button 2 released!");
      pause();
    }
    // Aktualizace posledního stavu tlačítka
    lastButton2_State = button2_State;
  }
}

void reloadButtonPressed()
{
  reloadFile = random(8, 9+1);
  exe_cmd(0x0F, 1, reloadFile);

  if (BulletsCount =< 0) {}
    BulletsCount = 95;
  } else {
    BulletsCount--;
  }
  //exe_cmd(0x0D, 0, 0);
  Playing = true;
}

void processVolume()
{
  if (useSoftVolumeRotaryEncoder)
  {
    doSoftVolumeRotaryEncoder();
  }

  if (newVolume != volume) {
    if (newVolume > volume)
    {
      Serial.print("Volume up: ");
      volumeINC();
    } else {
      Serial.print("Volume down: ");
      volumeDEC();
    }

    newVolume = volume;
    ledState = !ledState;
    digitalWrite(led_Pin, ledState ? HIGH : LOW);

    Serial.println(volume);
  }
}

void doSoftVolumeRotaryEncoder()
{
  //if (millis() - lastVolumeEncoderRun < debounceTreshold) return;

  lastVolumeEncoderRun = millis();

  rotaryMasterValue = digitalRead(rotaryS1_Pin);
  if (oldRotaryMasterValue == rotaryMasterValue) return;

  oldRotaryMasterValue = rotaryMasterValue;
  doInternalVolumeDecisions();
}



byte N0 = 125;
byte N1 = 48;
byte N2 = 110;
byte N3 = 122;
byte N4 = 51;
byte N5 = 91;
byte N6 = 95;
byte N7 = 112;
byte N8 = 127;
byte N9 = 115;

byte numbers[10] = {N0,N1,N2,N3,N4,N5,N6,N7,N8,N9};

void displayNumbers(int dozens, int units)
{
  // ST_CP LOW to keep LEDs from changing while reading serial data
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, LSBFIRST, numbers[units]);
  shiftOut(dataPin, clockPin, LSBFIRST, numbers[dozens]);

  // ST_CP HIGH change LEDs
  digitalWrite(latchPin, HIGH);
}

void displayBullets()
{
  if (lastBulletsCount == bulletsCount) return;
  lastBulletsCount = bulletsCount;

  displayNumbers(bulletsCount / 10, bulletsCount % 10);
}

void displayVolume()
{
  if (lastVolume == volume) return;
  lastVolume = volume;

  displayNumbers(volume / 10, volume % 10);
}




void initMe()
{
  Serial.println("Init...");
  enableACK();
  exe_cmd(CMD_INIT, 0, 0x02);
  //exe_cmd(CMD_INIT, 0, DFPLAYER_DEVICE_U_DISK);
  
  //exe_cmd(0x47, 0, 2); // 1=USB, 2=SD
  //available();

  // Specify playback - source(0/1/2/3/4) U/TF/AUX/SLEEP/FLASH
  //exe_cmd(CMD_PLAYBACK_SOURCE, 0, 0);

  //exe_cmd(CMD_SET_FOLDER_TO_PLAY, 0, 1);
  exe_cmd(CMD_SET_VOLUME, 0, volume);
  //exe_cmd(CMD_REPEAT_PLAY, 0, 0);
  //exe_cmd(0x03, 1, 1);
}

void stop()
{
  exe_cmd(0x16,0,1);
}

void playFirst()
{
  //exe_cmd(CMD_NORMAL_OPERATE, 0, 1);
  exe_cmd(CMD_RESUME, 0, 0);
}

void pause()
{
  exe_cmd(0x0E,0,0);
}

void play()
{
  exe_cmd(0x0D,0,1);
}

void next()
{
  exe_cmd(0x01,0,1);
}

void previous()
{
  exe_cmd(0x02,0,1);
}

void volumeINC()
{
  volume++;
  if(volume > 30)
  {
    volume = 30;
  }
  exe_cmd(0x06, 0, volume);
}

void volumeDEC()
{
  volume--;
  if(volume < 0)
  {
    volume = 0;
  }
  exe_cmd(0x06, 0, volume);
}

void equalizer()
{
  eqset = eqset+1;
  if(eqset == 6)
  {
    eqset = 0;
  }
  exe_cmd(0x07, 0 ,eqset);
}

// void exe_cmd(byte CMD, byte Par1, byte Par2)
// {
//   word checksum = 0xFFFF-(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
//   //int16_t checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
//   byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
//   for (byte x=0; x<10; x++)
//   {
//     mySerial.write(Command_line[x]);
//   }
// }

void exe_cmd(byte CMD, byte Par1, byte Par2) {
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
