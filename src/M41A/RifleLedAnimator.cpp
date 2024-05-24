#include "RifleLedAnimator.h"

// Constructor for RifleLedAnimator, initializes member variables
RifleLedAnimator::RifleLedAnimator()
  : ledPin(0), animationTime(0), flashTime(0), isAnimating(false), ledState(false), ledFlipMillis(0)
{
}

// Set up the rifle LED animator with the specified pin, animation time, and flash time
void RifleLedAnimator::setup(int rifleLedPin, int timeForOneBulet, int timeForFlash) {
    ledPin = rifleLedPin;
    animationTime = timeForOneBulet;
    flashTime = timeForFlash;

    isAnimating = false;
    ledState = false;

    pinMode(ledPin, OUTPUT);
    ledOff();  // Ensure the LED is off initially
}

// Update the state of the rifle LED based on the current time and animation settings
void RifleLedAnimator::update() {
    if (!isAnimating) {
        ledOff();  // If not animating, ensure the LED is off
        return;
    }

    unsigned long currentMillis = millis();

    // Checking the status of the LED and the time for switching
    if (ledState) {
        if (currentMillis - ledFlipMillis >= flashTime) {
            ledOff();  // Turn off the LED after the flash time
            ledFlipMillis = currentMillis;  // Reset the timer for the next cycle
        }
    } else {
        if (currentMillis - ledFlipMillis >= animationTime - flashTime) {
            ledOn();  // Turn on the LED for the next flash
            ledFlipMillis = currentMillis;  // Reset the timer for the next cycle
        }
    }
}

// Start the rifle LED animation
void RifleLedAnimator::start() {
    if (isAnimating) return;  // Prevent starting if already animating

    ledFlipMillis = millis();  // Record the start time
    isAnimating = true;        // Set the animation flag to true
    ledOn();                   // Turn on the LED initially
}

// Stop the rifle LED animation
void RifleLedAnimator::stop() {
    isAnimating = false;  // Set the animation flag to false
    ledOff();             // Ensure the LED is off
}

// Turn off the rifle LED
void RifleLedAnimator::ledOff() {
  ledState = false;          // Set the LED state to off
  digitalWrite(ledPin, LOW); // Set the LED pin to LOW
}

// Turn on the rifle LED
void RifleLedAnimator::ledOn() {
  ledState = true;           // Set the LED state to on
  digitalWrite(ledPin, HIGH);// Set the LED pin to HIGH
}
