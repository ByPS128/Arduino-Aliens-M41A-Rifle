#pragma once

#include <Arduino.h>

class GranadeLedAnimator {
public:
    void setup(int granadePwmLedPin, int timeToFullFadeOutInMs);
    void update();
    void start();
    void stop();

private:
    int ledPin;
    int fadeOutTime;
    unsigned long startTime;
    bool isAnimating;

private:
    void ledOff();
};
