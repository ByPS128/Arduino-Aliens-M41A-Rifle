#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX
const int buttonPin = 4; 
int lastButtonState = HIGH;

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

# define _DEBUG 1

// Create the Player object
DFRobotDFPlayerMini player;

void buttonPressed() {
  player.play(1);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial, 2, true)) {
    Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(5);
    player.EQ(DFPLAYER_EQ_BASS);
    //player.outputDevice(0);

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {
  // Čtení stavu tlačítka
  int buttonState = digitalRead(buttonPin);

  // Kontrola, zda došlo ke změně stavu tlačítka a zda tlačítko bylo stisknuto
  if (buttonState != lastButtonState) {  // Kontrola, zda došlo ke změně stavu
    if (buttonState == LOW) {
      Serial.println("Button pressed!");
      buttonPressed();  // Zavolání funkce při stisku tlačítka
    } else {
      Serial.println("Button released!");
    }
    // Aktualizace posledního stavu tlačítka
    lastButtonState = buttonState;
  }

  // Krátké zpoždění pro zamezení nadměrného zpracování
  delay(1);
}