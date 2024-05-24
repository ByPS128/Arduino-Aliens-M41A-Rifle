#include <Arduino.h>
#include <EEPROM.h>
#include "M41aSimulator.h"
#include "AppConstants.h"

M41aSimulator::M41aSimulator() :
    oldRotaryMasterValue(1), bulletsCount(95), newVolume(20), volumeDisplayed(false), 
    fireMillis(0), ledBlinkMillis(0), weaponReadyPlayed(false), isVolumeEpromWriteFlag(false), 
    isGranadeLoaded(false)
{
}

void M41aSimulator::setup()
{
  #ifdef _DEBUG
  Serial.begin(9600);
  Serial.println("Booting..");
  #endif

  readIsVolumeEpromWriteFlag();
  byte epromVolume = readVolumeFromEprom();
  newVolume = epromVolume;

  display.setup(latch_Pin, clock_Pin, data_Pin, true);

  randomSeed(millis());

  rifleLedAnimator.setup(ledRiffle_Pin, FIRE_COUNTDOWN_INTERVAL, RIFLE_FIRE_FLASH_TIMEOUT);
  granadeLedAnimator.setup(ledGranade_Pin, GRANADE_FLASH_FADEOUT_TIME);
  displayAnimator.setup(&display);
  displayAnimator.animateClockwiseInit(50);

  button1.setup(button1_Pin, rifleFireCallback, rifleStopCallback, this);
  button2.setup(button2_Pin, magazineReloadCallback, nullptr, this);
  button3.setup(button3_Pin, cockCallback, nullptr, this);
  button4.setup(button4_Pin, grandeFireCallback, nullptr, this);
  rotaryButton.setup(rotaryButton_Pin, LONG_PRESS_TIME, volumeButtonShortPressCallback, volumeButtonLongPressCallback, this);

  pinMode(rotaryS1_Pin, INPUT);
  pinMode(rotaryS2_Pin, INPUT);
  pinMode(rotaryButton_Pin, INPUT);

  #ifdef _DEDBUG
  Serial.println("M41A simulator ready");
  #endif

  player.setup(epromVolume);
  player.setVolume(epromVolume);
}

void M41aSimulator::update()
{
  player.update();
  rifleLedAnimator.update();
  granadeLedAnimator.update();
  displayAnimator.update();

  if ((displayAnimator.getCurrentAnimation() == CLOCKWISE_INIT || displayAnimator.getCurrentAnimation() == COUNTERCLOCKWISE_INIT) &&  displayAnimator.getElapsedTime() > 3000) {
    displayAnimator.stop();
    bulletsCount = 0;
    cock();
  }

  button1.update();
  button2.update();
  button3.update();
  button4.update();
  rotaryButton.update();
  
  // if (!weaponReadyPlayed) {
  //   weaponReadyPlayed = true;
  //   player.playWeaponReady();
  // }

  if (button1.isPressed() && bulletsCount > 0)
  {
    unsigned long currentMillis = millis();

    if (fireMillis < currentMillis) {
      fireMillis = currentMillis + FIRE_COUNTDOWN_INTERVAL;
      bulletsCount--;
      displayBullets();
      if (bulletsCount == 0)
      {
        rifleLedAnimator.stop();
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


void M41aSimulator::onAnimationCompletecCallback(M41aSimulator* instance)
{
  instance->cock();
}

void M41aSimulator::rifleFireCallback(M41aSimulator* instance)
{
  instance->rifleFire();
}

void M41aSimulator::rifleStopCallback(M41aSimulator* instance)
{
  instance->rifleStop();
}

void M41aSimulator:: cockCallback(M41aSimulator* instance)
{
  instance->cock();
}

void M41aSimulator:: magazineReloadCallback(M41aSimulator* instance)
{
  instance->magazineReload();
}

void M41aSimulator:: grandeFireCallback(M41aSimulator* instance)
{
  instance->granadeLoadOrFire();
}

void M41aSimulator:: volumeButtonShortPressCallback(M41aSimulator* instance)
{
  instance->volumeButtonShortPress();
}

void M41aSimulator:: volumeButtonLongPressCallback(M41aSimulator* instance)
{
  instance->volumeButtonLongPress();
}


void M41aSimulator::granadeLoadOrFire()
{
  if (isGranadeLoaded) {
      granadeFire();
  } else {
      granadeLoad();
  }
  isGranadeLoaded = !isGranadeLoaded;
}

void M41aSimulator::granadeLoad()
{
  player.playGranadeLoad();
}

void M41aSimulator::granadeFire()
{
  player.playGranadeFire();
  granadeLedAnimator.ledAnimationStart();
}

void M41aSimulator::granadeExplosion()
{
  player.playGranadeExplosion();
}

void M41aSimulator::rifleFire()
{
  if (bulletsCount <= 0) {
    player.playEmptyMagazine();
    return;
  }

  player.playRifleFire(bulletsCount);
  rifleLedAnimator.start();
}

void M41aSimulator::rifleStop()
{
  rifleLedAnimator.stop(); 
  player.stop();
}

void M41aSimulator::cock()
{
  player.playCock();
  rifleLedAnimator.stop();
  displayAnimator.animateTransition(bulletsCount, MAX_BULETS_COUNT, 60);
  bulletsCount = MAX_BULETS_COUNT;
}

void M41aSimulator::magazineReload()
{
  player.playMagazineReload();
  rifleLedAnimator.stop();
  displayAnimator.animateTransition(bulletsCount, MAX_BULETS_COUNT, 60);
  bulletsCount = MAX_BULETS_COUNT;
}

void M41aSimulator::magazineEject()
{
  player.playMagazineEject();
  bulletsCount = 0;
  rifleLedAnimator.stop();
  displayBullets();
}

void M41aSimulator::magazineLoad()
{
  player.playMagazineLoad();
  bulletsCount = MAX_BULETS_COUNT;
  rifleLedAnimator.stop();
  displayBullets();
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
  doSoftVolumeRotaryEncoder();

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
