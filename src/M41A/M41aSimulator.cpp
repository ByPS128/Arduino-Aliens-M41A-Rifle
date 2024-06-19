#include <Arduino.h>
#include <EEPROM.h>
#include "M41aSimulator.h"
#include "AppConstants.h"

M41aSimulator::M41aSimulator() :
    oldRotaryMasterValue(1), newVolume(20), volumeDisplayed(false),
    fireMillis(0), ledBlinkMillis(0), weaponReadyPlayed(false), isVolumeEpromWriteFlag(false)
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

  display.setup(LATCH_PIN, CLOCK_PIN, DATA_PIN, true);

  randomSeed(millis());

  rifleLedAnimator.setup(LED_RIFLE_PIN, FIRE_COUNTDOWN_INTERVAL, RIFLE_FIRE_FLASH_TIMEOUT);
  grenadeLedAnimator.setup(LED_GRENADE_PIN, GRENADE_FLASH_FADEOUT_TIME);
  displayAnimator.setup(&display);
  displayAnimator.animateClockwiseLoading(50);

  rifleTriggerButton.setup(RIFLE_TRIGGER_PIN, rifleFireCallback, rifleStopCallback, this);
  magazineReloadButton.setup(MAGAZINE_RELOAD_PIN, magazineReloadCallback, nullptr, this);
  rifleCockButton.setup(RIFLE_COCK_PIN, rifleCockCallback, nullptr, this);
  grenadeTriggerButton.setup(GRENADE_TRIGGER_PIN, grenadeFireCallback, nullptr, this);
  rotaryButton.setup(ROTARY_BUTTON_PIN, LONG_PRESS_TIME, volumeButtonShortPressCallback, volumeButtonLongPressCallback, this);

  pinMode(ROTARY_S1_PIN, INPUT);
  pinMode(ROTARY_S2_PIN, INPUT);
  pinMode(ROTARY_BUTTON_PIN, INPUT);

  #ifdef _DEDBUG
  Serial.println("M41A simulator ready");
  #endif

  player.setup(epromVolume);
  player.setVolume(epromVolume);
  state.setBulletsCount(MAX_BULLETS_COUNT);
}

void M41aSimulator::update()
{
  player.update();
  rifleLedAnimator.update();
  grenadeLedAnimator.update();
  displayAnimator.update();

  if ((displayAnimator.getCurrentAnimation() == AnimationType::CLOCKWISE_LOADING || displayAnimator.getCurrentAnimation() == AnimationType::COUNTERCLOCKWISE_LOADING) &&  displayAnimator.getElapsedTime() > VOLUME_DISPLAY_TIMEOUT) {
    displayAnimator.stop();
    state.setBulletsCount(0);
    rifleCock();
  }

  rifleTriggerButton.update();
  magazineReloadButton.update();
  rifleCockButton.update();
  grenadeTriggerButton.update();
  rotaryButton.update();
  
  if (rifleTriggerButton.isPressed() && state.getBulletsCount() > 0)
  {
    unsigned long currentMillis = millis();

    if (fireMillis < currentMillis) {
      fireMillis = currentMillis + FIRE_COUNTDOWN_INTERVAL;
      state.decrementBulletsCount();
      displayBullets();
      if (state.getBulletsCount() == 0)
      {
        rifleLedAnimator.stop();
        player.playEmptyMagazine();
      }
    }
  }

  // If the volume display timeout expires, display the bullet count again
  if (volumeDisplayed && lastVolumeEncoderRun + VOLUME_DISPLAY_TIMEOUT < millis()) {
    volumeDisplayed = false;
    displayBullets();
  }

  processVolume();
}


void M41aSimulator::onAnimationCompleteCallback(M41aSimulator* instance)
{
  instance->rifleCock();
}

void M41aSimulator::rifleFireCallback(M41aSimulator* instance)
{
  instance->rifleFire();
}

void M41aSimulator::rifleStopCallback(M41aSimulator* instance)
{
  instance->rifleStop();
}

void M41aSimulator::rifleCockCallback(M41aSimulator* instance)
{
  instance->rifleCock();
}

void M41aSimulator::magazineReloadCallback(M41aSimulator* instance)
{
  instance->magazineReload();
}

void M41aSimulator::grenadeFireCallback(M41aSimulator* instance)
{
  instance->grenadeLoadOrFire();
}

void M41aSimulator::volumeButtonShortPressCallback(M41aSimulator* instance)
{
  instance->volumeButtonShortPress();
}

void M41aSimulator::volumeButtonLongPressCallback(M41aSimulator* instance)
{
  instance->volumeButtonLongPress();
}


void M41aSimulator::grenadeLoadOrFire()
{
  if (state.isGrenadeLoaded()) {
      grenadeFire();
  } else {
      grenadeLoad();
  }
  state.toggleGrenadeLoaded();
}

void M41aSimulator::grenadeLoad()
{
  player.playGranadeLoad();
}

void M41aSimulator::grenadeFire()
{
  player.playGranadeFire();
  grenadeLedAnimator.start();
}

void M41aSimulator::grenadeExplosion()
{
  player.playGranadeExplosion();
}

void M41aSimulator::rifleFire()
{
  if (state.getBulletsCount() <= 0) {
    player.playEmptyMagazine();
    return;
  }

  player.playRifleFire(state.getBulletsCount());
  rifleLedAnimator.start();
}

void M41aSimulator::rifleStop()
{
  rifleLedAnimator.stop(); 
  if (state.getBulletsCount() > 0) {
    player.stop();
  }
}

void M41aSimulator::rifleCock()
{
  player.playCock();
  rifleLedAnimator.stop();
  displayAnimator.animateTransition(state.getBulletsCount(), MAX_BULLETS_COUNT, TRANSITION_DURATION_PER_10_UNITS);
  state.setBulletsCount(MAX_BULLETS_COUNT);
}

void M41aSimulator::magazineReload()
{
  player.playMagazineReload();
  rifleLedAnimator.stop();
  displayAnimator.animateTransition(state.getBulletsCount(), MAX_BULLETS_COUNT, TRANSITION_DURATION_PER_10_UNITS);
  state.setBulletsCount(MAX_BULLETS_COUNT);
}

void M41aSimulator::magazineEject()
{
  player.playMagazineEject();
  state.setBulletsCount(0);
  rifleLedAnimator.stop();
  displayBullets();
}

void M41aSimulator::magazineLoad()
{
  player.playMagazineLoad();
  state.setBulletsCount(MAX_BULLETS_COUNT);
  rifleLedAnimator.stop();
  displayBullets();
}

void M41aSimulator::volumeButtonShortPress()
{
  // Display volume
  if (player.getPlaying()) return;
  displayVolume();
}

void M41aSimulator::volumeButtonLongPress()
{
  // Save volume to EEPROM
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

  // Volume changed
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
  }

  newVolume = player.getVolume();
  #ifdef _DEBUG
  Serial.println(newVolume);
  #endif
}

void M41aSimulator::doSoftVolumeRotaryEncoder()
{
  rotaryMasterValue = digitalRead(ROTARY_S1_PIN);
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
  int rotaryValue = digitalRead(ROTARY_S2_PIN);
  if (rotaryValue == 1)
  {
    newVolume = volume + 1;
  } else if (rotaryValue == 0) {
    newVolume = volume - 1;
  }
}

void M41aSimulator::displayBullets()
{
  volumeDisplayed = false;
  display.displayNumbers(state.getBulletsCount() / 10, state.getBulletsCount() % 10);
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
    return DEFAULT_VOLUME_VALUE; // default volume value
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