#pragma once

#include <Arduino.h>

class RifleLedAnimator {
public:
    RifleLedAnimator();
    void setup(int rifleLedPin, int timeForOneBulet, int timeForFlash);
    void update();
    void start();
    void stop();

private:
    int ledPin;
    int animationTime;
    int flashTime;
    bool isAnimating;
    bool ledState;
    unsigned long ledFlipMillis;

    void ledOn();
    void ledOff();
};
