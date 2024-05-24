#include "RifleLedAnimator.h"

RifleLedAnimator::RifleLedAnimator()
  : ledPin(0), animationTime(0), flashTime(0), isAnimating(false), ledState(false), ledFlipMillis(0)
{
}

void RifleLedAnimator::setup(int rifleLedPin, int timeForOneBulet, int timeForFlash) {
    ledPin = rifleLedPin;
    animationTime = timeForOneBulet;
    flashTime = timeForFlash;

    isAnimating = false;
    ledState = false;

    pinMode(ledPin, OUTPUT);
    ledOff();
}

void RifleLedAnimator::update() {
    if (!isAnimating) {
        ledOff();
        return;
    }

    unsigned long currentMillis = millis();

    // Kontrola stavu LED a času pro přepnutí
    if (ledState) {
        if (currentMillis - ledFlipMillis >= flashTime) {
            ledOff();
            ledFlipMillis = currentMillis;  // Resetování časovače pro příští cyklus
        }
    } else {
        if (currentMillis - ledFlipMillis >= animationTime - flashTime) {
            ledOn();
            ledFlipMillis = currentMillis;  // Resetování časovače pro příští cyklus
        }
    }
}

void RifleLedAnimator::start() {
    if (isAnimating) return;

    ledFlipMillis = millis();
    isAnimating = true;
    ledOn();
}

void RifleLedAnimator::stop() {
    isAnimating = false;
    ledOff();
}

void RifleLedAnimator::ledOff() {
  ledState = false;
  digitalWrite(ledPin, LOW);
}

void RifleLedAnimator::ledOn() {
  ledState = true;
  digitalWrite(ledPin, HIGH);
}
