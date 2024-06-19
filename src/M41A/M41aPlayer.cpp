#include "M41aPlayer.h"

// Constructor for M41aPlayer
M41aPlayer::M41aPlayer() {
}

// Setup the M41aPlayer with the specified volume
void M41aPlayer::setup(byte newVolume) {
  dfPlay.setup(newVolume, dfPlay.ACK_DISABLED);
#ifdef _DEBUG
  Serial.println("M41A player ready");
#endif
}

// Update the state of the M41aPlayer, handle scheduled explosions
void M41aPlayer::update() {
  dfPlay.update();
  if (explosionScheduled && millis() >= explosionTime) {
    dfPlay.play(1, SAMPLE_GRENADE_EXPLOSION);
    explosionScheduled = false;
  }
}

// Stop the player
void M41aPlayer::stop() {
  if (getPlaying()) {
    dfPlay.stop();
  }
}

// Check if the player is currently playing
bool M41aPlayer::getPlaying() {
  return dfPlay.getPlaying();
}

// Get the current volume
byte M41aPlayer::getVolume() {
  return dfPlay.getVolume();
}

// Set a new volume
void M41aPlayer::setVolume(byte newVolume) {
  dfPlay.setVolume(newVolume);
}

// Increase the volume by one step and play a click sound
void M41aPlayer::volumeIncrement() {
  dfPlay.volumeIncrement();
  playClick();
}

// Decrease the volume by one step and play a click sound
void M41aPlayer::volumeDecrement() {
  dfPlay.volumeDecrement();
  playClick();
}

// Set the equalizer to a specific type
void M41aPlayer::setEqualizer(byte equalizerType) {
  dfPlay.setEqualizer(equalizerType);
}

// Reset the grenade explosion scheduling
void M41aPlayer::resetGranadeExplositon() {
  explosionTime = 0;
  explosionScheduled = false;
}

// Play a rifle fire sound based on the number of bullets remaining
void M41aPlayer::playRifleFire(int bulletsCount) {
  resetGranadeExplositon();
  if (bulletsCount > 0) {
    int fileToPlay = lastFireInstrument;
    int stopper = 0;
    while (lastFireInstrument == fileToPlay && stopper++ < 3) {
      fileToPlay = random(SAMPLE_RIFLE_FIRE_FIRST, SAMPLE_RIFLE_FIRE_LAST + 1);
    }
    dfPlay.play(fileToPlay);
  } else {
    playEmptyMagazine();
  }
}

// Play a cocking sound
void M41aPlayer::playCock() {
  int fileToPlay = random(SAMPLE_COCK_FIRST, SAMPLE_COCK_LAST + 1);
  dfPlay.play(fileToPlay);
}

// Play a magazine reload sound
void M41aPlayer::playMagazineReload() {
  int fileToPlay = random(SAMPLE_MAGAZINE_RELOAD_FIRST, SAMPLE_MAGAZINE_RELOAD_LAST + 1);
  dfPlay.play(fileToPlay);
}

// Play a magazine eject sound
void M41aPlayer::playMagazineEject() {
  int fileToPlay = random(SAMPLE_MAGAZINE_EJECT_FIRST, SAMPLE_MAGAZINE_EJECT_LAST + 1);
  dfPlay.play(fileToPlay);
}

// Play a magazine load sound
void M41aPlayer::playMagazineLoad() {
  int fileToPlay = random(SAMPLE_MAGAZINE_LOAD_FIRST, SAMPLE_MAGAZINE_LOAD_LAST + 1);
  dfPlay.play(fileToPlay);
}

// Play an empty magazine sound
void M41aPlayer::playEmptyMagazine() {
  int fileToPlay = lastEmptyMagazineInstrument;
  int stopper = 0;
  while (lastEmptyMagazineInstrument == fileToPlay && stopper++ < 3) {
    fileToPlay = random(SAMPLE_EMPTY_MAGAZINE_FIRST, SAMPLE_EMPTY_MAGAZINE_LAST + 1);
  }
  dfPlay.play(fileToPlay);
}

// Play a grenade load sound
void M41aPlayer::playGranadeLoad() {
  explosionScheduled = false;  
  dfPlay.play(1, SAMPLE_GRENADE_LOAD);
}

// Play a grenade fire sound and schedule an explosion
void M41aPlayer::playGranadeFire() {
  dfPlay.play(1, SAMPLE_GRENADE_FIRE);
  int delayTime = random(GRENADE_BALLISTIC_DELAY_MIN, GRENADE_BALLISTIC_DELAY_MAX);
  explosionTime = millis() + delayTime;
  explosionScheduled = true;  
}

// Play a grenade explosion sound
void M41aPlayer::playGranadeExplosion() {
  resetGranadeExplositon();
  dfPlay.play(1, SAMPLE_GRENADE_EXPLOSION);
}

// Play a click sound
void M41aPlayer::playClick() {
  return;
  if (dfPlay.getPlaying()) {
    return;
  }
  dfPlay.play(SAMPLE_CLICK);
}

// Play a volume saved sound
void M41aPlayer::playVolumeSaved() {
  dfPlay.play(SAMPLE_VOLUME_SAVED);
}

// Play a volume up sound
void M41aPlayer::playVolumeUp() {
  dfPlay.play(SAMPLE_VOLUME_UP);
}

// Play a volume down sound
void M41aPlayer::playVolumeDown() {
  dfPlay.play(SAMPLE_VOLUME_DOWN);
}

// Play a card inserted sound
void M41aPlayer::playCardInserted() {
  dfPlay.play(SAMPLE_CARD_INSERTED);
}

// Play a card removed sound
void M41aPlayer::playCardRemoved() {
  dfPlay.play(SAMPLE_CARD_REMOVED);
}

// Play a low battery sound
void M41aPlayer::playLowBattery() {
  dfPlay.play(SAMPLE_LOW_BATTERY);
}

// Play a weapon ready sound
void M41aPlayer::playWeaponReady() {
  dfPlay.play(SAMPLE_WEAPON_READY);
}
