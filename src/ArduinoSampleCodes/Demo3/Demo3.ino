#include <DFPlayer_Mini_Mp3.h>

const int buttonPin = 4; 
int lastButtonState = HIGH;

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

  pinMode(buttonPin, INPUT_PULLUP);

  delay(1000);
  mp3_set_serial (Serial); //set Serial for DFPlayer-mini mp3 module 
  delay(1);
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
      buttonPressed();  // Zavolání funkce při stisku tlačítka
    } else {
    }
    // Aktualizace posledního stavu tlačítka
    lastButtonState = buttonState;
  }

  delay(5);
}
