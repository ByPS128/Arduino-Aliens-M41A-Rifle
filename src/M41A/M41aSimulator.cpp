#include <Arduino.h>
#include <EEPROM.h>
#include "M41aSimulator.h"
#include "AppConstants.h"

M41aSimulator::M41aSimulator()
  : lastButton1_State(HIGH), lastButton2_State(HIGH), lastButtonVolume_State(HIGH), oldRotaryMasterValue(1), 
    bulletsCount(95), newVolume(20), volumeDisplayed(false), fireMillis(0), ledBlinkMillis(0), weaponReadyPlayed(false),
    button1IsPressed(false), button2IsPressed(false), buttonVolumeIsPressed(false), isVolumeEpromWriteFlag(false)
{
}

void M41aSimulator::setup()
{
  #ifdef _DEBUG
  Serial.begin(9600);
  Serial.println("Booting..");
  #endif

  display.setup(latch_Pin, clock_Pin, data_Pin, true);
  display.displaySegments(display.NNone, display.NNone);

  readIsVolumeEpromWriteFlag();
  byte epromVolume = readVolumeFromEprom();
  player.setup(epromVolume);
  newVolume = epromVolume;

  randomSeed(millis());

  pinMode(button1_Pin, INPUT_PULLUP);
  digitalWrite(button1_Pin, HIGH);

  pinMode(button2_Pin, INPUT_PULLUP);
  digitalWrite(button2_Pin, HIGH);

  pinMode(led_Pin, OUTPUT);

  useSoftVolumeRotaryEncoder = true || (digitalPinToInterrupt(rotaryS1_Pin) != rotaryS1_Pin);
  #ifdef _DEDBUG
  Serial.print("Volume rotary encoder uses ");
  Serial.print(useSoftVolumeRotaryEncoder ? "soft" : "interupt");
  Serial.println(" read.");
  #endif
  if (!useSoftVolumeRotaryEncoder) {
    //attachInterrupt(digitalPinToInterrupt(rotaryS1_Pin), doInterruptVolumeRotaryEncoder, FALLING);
    //oldRotaryMasterValue = digitalRead(rotaryS1_Pin);
  }

  pinMode(rotaryS1_Pin, INPUT);
  pinMode(rotaryS2_Pin, INPUT);
  pinMode(rotaryButton_Pin, INPUT);

  #ifdef _DEDBUG
  Serial.println("M41A simulator ready");
  #endif

  displayBullets();
  ledOff();
}

void M41aSimulator::update()
{
  player.update();

  // if (!weaponReadyPlayed) {
  //   weaponReadyPlayed = true;
  //   player.playWeaponReady();
  // }

  doButton1();
  doButton2();
  doVolumeButton();

  if (button1IsPressed && bulletsCount > 0)
  {
    unsigned long currentMillis = millis();
    if (ledBlinkMillis < currentMillis) {
      ledBlinkMillis = currentMillis + LED_BLINK_INTERVAL;
      ledTurn();
    }

    if (fireMillis < currentMillis) {
      fireMillis = currentMillis + FIRE_COUNTDOWN_INTERVAL;
      bulletsCount--;
      displayBullets();
      if (bulletsCount == 0)
      {
        ledOff();
        player.playEmptyMagazine();
      }
    }
  }

  // Jakmile vyprší timeout zobrazení hlasitosti, znovu se zobrazí počet nábojů.
  if (volumeDisplayed && lastVolumeEncoderRun + VolumeDisplayTimeout < millis()) {
    volumeDisplayed = false;
    displayBullets();
  }

  processVolume();
}

void M41aSimulator::doButton1()
{
  int button1_State = digitalRead(button1_Pin);
  // Kontrola, zda došlo ke změně stavu
  if (button1_State != lastButton1_State) {
    button1IsPressed = button1_State == LOW;
    if (button1IsPressed) {
      player.playFire(bulletsCount);
      #ifdef _DEBUG
      Serial.println("Button 1 pressed!");
      #endif
    } else {
      ledOff();
      player.pause();
      #ifdef _DEBUG
      Serial.println("Button 1 released!");
      #endif
    }

    // Aktualizace posledního stavu tlačítka
    lastButton1_State = button1_State;
  }

  // if (lastButton1_State == LOW && bulletsCount > 0)
  // {
  //   // Dokud je tlačítko stisknuto, probíhá:
  //   //  * odpočet nábojů v zásobníku, podle zvoleného intervalu pro odpočet
  //   //  * blikání ledkou, podle zvolenohé interfavlu pro blikání
  //   unsigned long currentMillis = millis();

  //   // Odpočet nábojů
  //   if (fireMillis < currentMillis) {
  //     fireMillis = currentMillis + FIRE_COUNTDOWN_INTERVAL;
  //     bulletsCount--;
  //     displayBullets();
  //     if (bulletsCount == 0)
  //     {
  //       ledOff();
  //       player.playEmptyMagazine();
  //     }
  //   }

  //   // Blikání ledkou
  //   if (ledBlinkMillis < currentMillis) {
  //     ledBlinkMillis = currentMillis + LED_BLINK_INTERVAL;
  //     ledTurn();
  //   }
  // }
}

void M41aSimulator::doButton2()
{
  int button2_State = digitalRead(button2_Pin);
  // Kontrola, zda došlo ke změně stavu
  if (button2_State != lastButton2_State) {
    button2IsPressed = button2_State == LOW;
    if (button2IsPressed)
    {
      magazineReload();
      #ifdef _DEBUG
      Serial.println("Button 2 pressed!");
      #endif
    } else {
      #ifdef _DEBUG
      Serial.println("Button 2 released!");
      #endif
    }

    // Aktualizace posledního stavu tlačítka
    lastButton2_State = button2_State;
  }
}

void M41aSimulator::cock()
{
  ledOff();
  player.playCock();
}

void M41aSimulator::magazineReload()
{
  player.playMagazineReload();
  bulletsCount = 95;
  ledOff();
  displayBullets();
}

void M41aSimulator::magazineEject()
{
  player.playMagazineEject();
  bulletsCount = 0;
  ledOff();
  displayBullets();
}

void M41aSimulator::magazineLoad()
{
  player.playMagazineLoad();
  bulletsCount = 95;
  ledOff();
  displayBullets();
}

// Metoda rozpoznává krátký a dlouhý stisk tlačítka na rotary enkodéru.
// Princip:
// při stisku začnu měřit čas
// pokud je tláčítko stále stisknuté a čas překoná long press timeout, jde o long press
// pokud tlačítko uvilním a nebyl ohandlovaný long press, jde o short press.
void M41aSimulator::doVolumeButton() {
    int buttonVolume_State = digitalRead(rotaryButton_Pin);
    static unsigned long buttonPressTime = 0; // Čas, kdy bylo tlačítko stisknuto
    static bool isPressHandled = false; // Byla již akce pro stisk zpracována?

    if (buttonVolume_State != lastButtonVolume_State) { // Detekce změny stavu tlačítka
        lastButtonVolume_State = buttonVolume_State;
        buttonVolumeIsPressed = buttonVolume_State == LOW; // Tlačítko bylo stisknuto ?

        if (buttonVolumeIsPressed) { 
            buttonPressTime = millis(); // Uložení času stisku
            isPressHandled = false; // Resetování flagu pro zpracování akce
            #ifdef _DEBUG
            Serial.println("Volume Button pressed!");
            #endif
        } else { // Tlačítko bylo uvolněno
            if (!isPressHandled) { // Kontrola, zda nebyla akce již zpracována
                volumeButtonShortPress(); // Zpracování krátkého stisku
                #ifdef _DEBUG
                Serial.println("Short Press detected!");
                #endif
            }
            #ifdef _DEBUG
            Serial.println("Volume Button released!");
            #endif
        }
    } else if (buttonVolume_State == LOW && !isPressHandled) {
        // Kontrola, zda doba stisku přesáhla limit pro dlouhý stisk
        if (millis() - buttonPressTime >= LONG_PRESS_TIME) {
            volumeButtonLongPress(); // Zpracování dlouhého stisku
            isPressHandled = true; // Nastavení flagu, že akce byla zpracována
            #ifdef _DEBUG
            Serial.println("Long Press detected!");
            #endif
        }
    }
}

void M41aSimulator::volumeButtonShortPress()
{
  // Kód pro zobrazení hlasitosti
  if (player.getPlaying()) return;
  displayVolume();
}

void M41aSimulator::volumeButtonLongPress()
{
  // Kód pro uložení hlasitosti
  writeVolumeToEprom(player.getVolume());
  if (player.getPlaying()) {
    return;
  }

  player.playVolumeSaved();  
}

void M41aSimulator::processVolume()
{
  if (useSoftVolumeRotaryEncoder)
  {
    // zjsitím jestli uživatel nepootočil s hlasitostí.
    doSoftVolumeRotaryEncoder();
  }

  int volume = player.getVolume();
  if (newVolume == volume)
  {
    return;
  }

  // Hlasitost se změnila
  if (newVolume > volume)
  {
    player.volumeIncrement();
    #ifdef _DEBUG
    Serial.print("Volume up: ");
    #endif
  } else {
    player.volumeDecrement();
    #ifdef _DEBUG
    Serial.print("Volume down: ");
    #endif
  }

  if (!player.getPlaying()) {
    displayVolume();
    // if (newVolume > volume)
    // {
    //   player.playVolumeUp();
    // } else {
    //   player.playVolumeDown();
    // }
  }

  newVolume = player.getVolume();
  #ifdef _DEBUG
  Serial.println(newVolume);
  #endif
}

void M41aSimulator::doSoftVolumeRotaryEncoder()
{
  rotaryMasterValue = digitalRead(rotaryS1_Pin);
  if (oldRotaryMasterValue == rotaryMasterValue) {
    return;
  }

  oldRotaryMasterValue = rotaryMasterValue;
  if (rotaryMasterValue == 0) return;
  doInternalVolumeDecisions();
}

// void M41aSimulator::doInterruptVolumeRotaryEncoder()
// {
//   rotaryMasterValue = digitalRead(rotaryS1_Pin);

//   noInterrupts();
//   doInternalVolumeDecisions();
//   interrupts();
// }

// Called as ISR, so don't do unnecessary things.
// U metody volené přerušením nemohu mít žádné parametry, proto si hlasitost musím vždy přečíst.
void M41aSimulator::doInternalVolumeDecisions()
{
  if (rotaryMasterValue == 0) return;

  int volume = player.getVolume();
  int rotaryValue = digitalRead(rotaryS2_Pin);
  if (rotaryValue == 1)
  {
    newVolume = volume + 1;
  } else if (rotaryValue == 0) {
    newVolume = volume - 1;
  }
}

void M41aSimulator::ledTurn()
{
  ledState = !ledState;
  digitalWrite(led_Pin, ledState ? HIGH : LOW);
}

void M41aSimulator::ledOff()
{
  ledState = false;
  digitalWrite(led_Pin, ledState ? HIGH : LOW);
}

void M41aSimulator::displayBullets()
{
  lastBulletsCount = bulletsCount;
  volumeDisplayed = false;

  display.displayNumbers(bulletsCount / 10, bulletsCount % 10);
}

void M41aSimulator::displayVolume()
{
  int volume = player.getVolume();
  if (volume > 0) {
    display.displayNumbers(volume / 10, volume % 10);
  } else {
    display.displaySegments(display.NMinus, display.NMinus);
  }

  lastVolumeEncoderRun = millis();
  volumeDisplayed = true;
}

bool M41aSimulator::readIsVolumeEpromWriteFlag() {
  byte storedFlag = EEPROM.read(INIT_FLAG_ADDRESS);
  isVolumeEpromWriteFlag = storedFlag == INITIALIZED_FLAG;
  return isVolumeEpromWriteFlag;
}

byte M41aSimulator::readVolumeFromEprom() {
  if (isVolumeEpromWriteFlag) {
    return EEPROM.read(VOLUME_ADDRESS);
  } else {
    return DEFAULT_VOLUME_VALUE; // default volume value;
  }
}

void M41aSimulator::writeVolumeToEprom(byte volume) {
  byte storedVolume = readVolumeFromEprom();
  if (storedVolume != volume) {
    EEPROM.write(VOLUME_ADDRESS, volume);
    if (!isVolumeEpromWriteFlag) {
      EEPROM.write(INIT_FLAG_ADDRESS, INITIALIZED_FLAG);
      isVolumeEpromWriteFlag = true;
    }
  }
}
