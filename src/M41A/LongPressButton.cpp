#include "LongPressButton.h"

LongPressButton::LongPressButton()
    : Button(), onShortPress(nullptr), onLongPress(nullptr), buttonPressTime(0), isPressHandled(false) {
}

void LongPressButton::setup(int pin, unsigned long longPressTime, Callback onShortPress, Callback onLongPress, void* context) {
    Button::setup(pin, nullptr, nullptr, context);
    this->onShortPress = onShortPress;
    this->onLongPress = onLongPress;
    this->longPressTime = longPressTime;
}

void LongPressButton::update() {
    Button::update();

    if (isPressed()) {
        if (!isPressHandled) {
            buttonPressTime = millis();
            isPressHandled = false;
        } else if (millis() - buttonPressTime >= longPressTime && onLongPress) {
            isPressHandled = true;
            onLongPress(context);
        }
    } else {
        if (!isPressHandled && onShortPress) {
            isPressHandled = true;
            onShortPress(context);
        }
    }
}
