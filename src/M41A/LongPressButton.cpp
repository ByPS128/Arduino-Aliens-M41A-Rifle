#include "LongPressButton.h"

LongPressButton::LongPressButton()
    : pin(-1), onShortPress(nullptr), onLongPress(nullptr), lastState(HIGH), pressed(false), buttonPressTime(0), isPressHandled(false) {
}

void LongPressButton::setup(int pin, long unsigned int longPressTime, Callback onShortPress, Callback onLongPress, void* context) {
    this->pin = pin;
    this->onShortPress = onShortPress;
    this->onLongPress = onLongPress;
    this->longPressTime = longPressTime;
    this->context = context;

    pinMode(pin, INPUT_PULLUP);
    digitalWrite(pin, HIGH);
}

void LongPressButton::update() {
    if (pin == -1) return; // Ensure setup has been called

    int currentState = digitalRead(pin);
    if (currentState != lastState) {
        pressed = currentState == LOW;
        if (pressed) {
            buttonPressTime = millis(); // Saving the button press time
            isPressHandled = false; // Reset flag for action processing
        } else { // The pusher has been released
            if (!isPressHandled &&onShortPress) {
              isPressHandled = true; // Setting the flag that the action has been processed
              onShortPress(context);// Checking if the action has not already been processed
            }
        }
        lastState = currentState;
    } else if (currentState == LOW && !isPressHandled) {
        // Checking if the press time has exceeded the long press limit
        if (millis() - buttonPressTime >= longPressTime && onLongPress) {
            isPressHandled = true; // Setting the flag that the action has been processed
            onLongPress(context); // Long press processing
        }
    }
}

bool LongPressButton::isPressed() {
    return pressed;
}
