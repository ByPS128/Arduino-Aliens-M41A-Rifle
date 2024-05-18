#include "GranadeLedAnimator.h"

void GranadeLedAnimator::setup(int granadePwmLedPin, int timeToFullFadeOutInMs) {
    ledPin = granadePwmLedPin;
    fadeOutTime = timeToFullFadeOutInMs;
    pinMode(ledPin, OUTPUT);
    ledOff();
}

void GranadeLedAnimator::ledAnimationStart() {
    startTime = millis();
    isAnimating = true;
    update();
}

void GranadeLedAnimator::update() {
    if (!isAnimating) {
      ledOff();
      return;
    }

    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (elapsedTime < fadeOutTime) {
        int brightness = 255 - (255 * elapsedTime / fadeOutTime);
        analogWrite(ledPin, brightness);
    } else {
        ledOff();
    }
}

void GranadeLedAnimator::ledOff() {
    isAnimating = false;
    analogWrite(ledPin, 0);
}
