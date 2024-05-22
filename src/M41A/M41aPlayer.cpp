#include "M41aPlayer.h"

M41aPlayer::M41aPlayer()
{
}

void M41aPlayer::setup(byte newVolume)
{
  dfPlay.setup(newVolume, dfPlay.ACK_DISABLED);

  #ifdef _DEBUG
  Serial.println("M41A player ready");
  #endif
}

void M41aPlayer::update()
{
  dfPlay.update();
  if (explosionScheduled && millis() >= explosionTime) {
    //dfPlay.playAdvertise(SAMPLE_GRANADE_EXPLOSION);
    dfPlay.play(1, SAMPLE_GRANADE_EXPLOSION);
    explosionScheduled = false;
  }
}

void M41aPlayer::stop()
{
  dfPlay.stop();
}

void M41aPlayer::pause()
{
  dfPlay.pause();
}

bool M41aPlayer::getPlaying()
{
  return dfPlay.getPlaying();
}

byte M41aPlayer::getVolume()
{
  return dfPlay.getVolume();
}

void M41aPlayer::setVolume(byte newVolume)
{
  dfPlay.setVolume(newVolume);
}

void M41aPlayer::volumeIncrement()
{
  dfPlay.volumeIncrement();
  playClick();
}

void M41aPlayer::volumeDecrement()
{
  dfPlay.volumeDecrement();
  playClick();
}

void M41aPlayer::setEqualizer(byte equalizerType)
{
  dfPlay.setEqualizer(equalizerType);
}

void M41aPlayer::resetGranadeExplositon()
{
  explosionTime = 0;
  explosionScheduled = false;
}

void M41aPlayer::playRifleFire(int bulletsCount)
{
  resetGranadeExplositon();
  if (bulletsCount > 0)
  {
    int fileToPlay = lastFireInstrument;
    int stopper = 0;
    while (lastFireInstrument == fileToPlay && stopper++ < 3)
    {
      fileToPlay = random(SAMPLE_RIFLE_FIRE_FIRST, SAMPLE_RIFLE_FIRE_LAST + 1);
    }

    dfPlay.play(fileToPlay);
  } else {
    playEmptyMagazine();
  }
}

void M41aPlayer::playCock()
{
  int fileToPlay = random(SAMPLE_COCK_FIRST, SAMPLE_COCK_LAST + 1);
  dfPlay.play(fileToPlay);
}

void M41aPlayer::playMagazineReload()
{
  int fileToPlay = random(SAMPLE_MAGAZINE_RELOAD_FIRST, SAMPLE_MAGAZINE_RELOAD_LAST + 1);
  dfPlay.play(fileToPlay);
}

void M41aPlayer::playMagazineEject()
{
  int fileToPlay = random(SAMPLE_MAGAZINE_EJECT_FIRST, SAMPLE_MAGAZINE_EJECT_LAST + 1);
  dfPlay.play(fileToPlay);
}

void M41aPlayer::playMagazineLoad()
{
  int fileToPlay = random(SAMPLE_MAGAZINE_LOAD_FIRST, SAMPLE_MAGAZINE_LOAD_LAST + 1);
  dfPlay.play(fileToPlay);
}

void M41aPlayer::playEmptyMagazine()
{
  int fileToPlay = lastEmptyMagazineInstrument;
  int stopper = 0;
  while (lastEmptyMagazineInstrument == fileToPlay && stopper++ < 3)
  {
    fileToPlay = random(SAMPLE_EMPTY_MAGAZINE_FIRST, SAMPLE_EMPTY_MAGAZINE_LAST + 1);
  }

  dfPlay.play(fileToPlay);
}

void M41aPlayer::playGranadeLoad()
{
  explosionScheduled = false;  
  //dfPlay.playAdvertise(SAMPLE_GRANADE_LOAD);
  dfPlay.play(1, SAMPLE_GRANADE_LOAD);
}

void M41aPlayer::playGranadeFire()
{
  //dfPlay.playAdvertise(SAMPLE_GRANADE_FIRE);
  dfPlay.play(1, SAMPLE_GRANADE_FIRE);
  int delayTime = random(GRANADE_BALISTIC_DELAY_MIN, GRANADE_BALISTIC_DELAY_MAX);
  explosionTime = millis() + delayTime;
  explosionScheduled = true;  
}

void M41aPlayer::playGranadeExplosion()
{
  resetGranadeExplositon();
  //dfPlay.playAdvertise(SAMPLE_GRANADE_EXPLOSION);
  dfPlay.play(1, SAMPLE_GRANADE_EXPLOSION);
}

void M41aPlayer::playClick()
{
  return;

  if (dfPlay.getPlaying()) {
    return;
  }

  dfPlay.play(SAMPLE_CLICK);
}

void M41aPlayer::playVolumeSaved()
{
  dfPlay.play(SAMPLE_VOLUME_SAVED);
}

void M41aPlayer::playVolumeUp()
{
  dfPlay.play(SAMPLE_VOLUME_UP);
}

void M41aPlayer::playVolumeDown()
{
  dfPlay.play(SAMPLE_VOLUME_DOWN);
}

void M41aPlayer::playCardInserted()
{
  dfPlay.play(SAMPLE_CARD_INSERTED);
}

void M41aPlayer::playCardRemoved()
{
  dfPlay.play(SAMPLE_CARD_REMOVED);
}

void M41aPlayer::playLowBattery()
{
  dfPlay.play(SAMPLE_LOW_BATTERY);
}

void M41aPlayer::playWeaponReady()
{
  dfPlay.play(SAMPLE_WEAPON_READY);
}
