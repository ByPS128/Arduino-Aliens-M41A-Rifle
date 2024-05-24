#include "Button.h"

Button::Button()
    : pin(-1), onPress(nullptr), onRelease(nullptr), lastState(HIGH), pressed(false) {
}

void Button::setup(int pin, Callback onPress, Callback onRelease, void* context) {
    this->pin = pin;
    this->onPress = onPress;
    this->onRelease = onRelease;
    this->context = context;

    pinMode(pin, INPUT_PULLUP);
    digitalWrite(pin, HIGH);
}

void Button::update() {
    if (pin == -1) return; // Ensure setup has been called

    int currentState = digitalRead(pin);
    if (currentState != lastState) {
        pressed = currentState == LOW;
        if (pressed && onPress) {
            onPress(context);
        } else if (!pressed && onRelease) {
            onRelease(context);
        }
        lastState = currentState;
    }
}

bool Button::isPressed() {
    return pressed;
}
