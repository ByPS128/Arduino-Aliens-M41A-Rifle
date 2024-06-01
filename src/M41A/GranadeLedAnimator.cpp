#include "GranadeLedAnimator.h"

// Set up the grenade LED animator with the specified pin and fade-out time
void GranadeLedAnimator::setup(int granadePwmLedPin, int timeToFullFadeOutInMs) {
    ledPin = granadePwmLedPin;
    fadeOutTime = timeToFullFadeOutInMs;
    pinMode(ledPin, OUTPUT);
    ledOff();  // Ensure the LED is off initially
}

// Start the grenade LED animation
void GranadeLedAnimator::start() {
    startTime = millis();  // Record the start time of the animation
    isAnimating = true;    // Set the animation flag to true
    update();              // Immediately update the LED state
}

// Update the grenade LED animation
void GranadeLedAnimator::update() {
    if (!isAnimating) {
        ledOff();  // If not animating, ensure the LED is off
        return;
    }

    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;  // Calculate elapsed time since the start

    // If the animation is still within the fade-out time
    if (elapsedTime < fadeOutTime) {
        // Calculate brightness based on elapsed time
        int brightness = 255 - (255 * elapsedTime / fadeOutTime);
        analogWrite(ledPin, brightness);  // Set LED brightness
    } else {
        ledOff();  // Turn off the LED if the fade-out time has elapsed
    }
}

void GranadeLedAnimator::stop() {
    ledOff();
}

// Turn off the grenade LED
void GranadeLedAnimator::ledOff() {
    isAnimating = false;  // Set the animation flag to false
    analogWrite(ledPin, 0);  // Set LED brightness to 0 (off)
}
