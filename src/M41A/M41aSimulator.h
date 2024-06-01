#pragma once

#include <Arduino.h>
#include "M41aPlayer.h"
#include "TwoDigitDisplay.h"
#include "RifleLedAnimator.h"
#include "GranadeLedAnimator.h"
#include "DisplayAnimator.h"
#include "Button.h"
#include "LongPressButton.h"
#include "SimulatorState.h"

class M41aSimulator {
private:
    SimulatorState state;
    unsigned long lastVolumeEncoderRun;
    int newVolume;
    int oldRotaryMasterValue;
    int rotaryMasterValue;
    int displayTime;
    unsigned long fireMillis;
    unsigned long ledBlinkMillis;
    bool volumeDisplayed;
    bool weaponReadyPlayed;
    bool isVolumeEpromWriteFlag;

private:
    Button rifleTriggerButton;
    Button magazineReloadButton;
    Button rifleCockButton;
    Button grenadeTriggerButton;
    LongPressButton rotaryButton;
    M41aPlayer player;
    TwoDigitDisplay display;
    RifleLedAnimator rifleLedAnimator;
    GranadeLedAnimator grenadeLedAnimator;
    DisplayAnimator displayAnimator;
    void (*afterPlaybackDelegate)() = nullptr;

public:
    M41aSimulator();
    void setup();
    void update();

private:
    // Callback methods
    static void onAnimationCompleteCallback(M41aSimulator* instance);
    static void rifleFireCallback(M41aSimulator* instance);
    static void rifleStopCallback(M41aSimulator* instance);
    static void rifleCockCallback(M41aSimulator* instance);
    static void magazineReloadCallback(M41aSimulator* instance);
    static void grenadeFireCallback(M41aSimulator* instance);
    static void volumeButtonShortPressCallback(M41aSimulator* instance);
    static void volumeButtonLongPressCallback(M41aSimulator* instance);

private:
    // Helper methods
    void doInternalVolumeDecisions();
    void grenadeLoadOrFire();
    void grenadeLoad();
    void grenadeFire();
    void grenadeExplosion();
    void rifleFire();
    void rifleStop();
    void rifleCock();
    void magazineReload();
    void magazineEject();
    void magazineLoad();
    void volumeButtonShortPress();
    void volumeButtonLongPress();
    void processVolume();
    void doSoftVolumeRotaryEncoder();
    void displayBullets();
    void displayVolume();
    bool readIsVolumeEpromWriteFlag();
    byte readVolumeFromEprom();
    void writeVolumeToEprom(byte volume);
};