#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX
const int buttonPin = 4; 
int lastButtonState = HIGH;

SoftwareSerial mySerial(PIN_MP3_RX, PIN_MP3_TX);

int volume = 15;
boolean Playing = false;

/*
1200 baudů – velmi pomalá rychlost, někdy používaná pro extrémně šumivé prostředí nebo při velmi dlouhých vzdálenostech.
2400 baudů – pomalá, ale stále použitelná pro jednoduché úlohy.
4800 baudů – opět poměrně pomalá, ale méně obvyklá.
9600 baudů – velmi běžná rychlost pro hobby projekty a jednoduché aplikace.
19200 baudů – střední rychlost, poskytuje dobrý kompromis mezi rychlostí a spolehlivostí.
38400 baudů – rychlejší, často používaná pro mírně náročnější aplikace.
57600 baudů – běžně používaná pro rychlejší komunikaci mezi zařízeními.
115200 baudů – jedna z nejběžnějších vysokých rychlostí, často používaná v moderních komunikacích mezi počítači a mikrokontroléry.
230400 baudů – velmi rychlá, používá se pro rychlý přenos dat.
460800 baudů a 921600 baudů – extrémně vysoké rychlosti, obvykle používané pouze ve specifických profesionálních aplikacích nebo když je hardware specificky navržen pro takové rychlosti.
*/

void setup ()
{
  Serial.begin(9600);
  Serial.println("OK");

  pinMode(buttonPin, INPUT_PULLUP);
  //digitalWrite(buttonPin, LOW);

  mySerial.begin(9600);
  delay(1000);
  mp3_set_serial (mySerial); //set Serial for DFPlayer-mini mp3 module 
  delay(100);
  mp3_set_volume (volume);  // set volume of speaker (0~30)
}

void buttonPressed() {
  mp3_play (1); //play 0001.mp3
  // delay(2 * 1000);
  // mp3_play (1); //play 0001.mp3
  // delay(10);
  // mp3_pause();
}

void loop ()
{
  int buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {  // Kontrola, zda došlo ke změně stavu
    if (buttonState == LOW) {
      Serial.println("Button pressed!");
      //buttonPressed();  // Zavolání funkce při stisku tlačítka
    } else {
      Serial.println("Button released!");
    }
    // Aktualizace posledního stavu tlačítka
    lastButtonState = buttonState;
  }

  delay(10);
}
