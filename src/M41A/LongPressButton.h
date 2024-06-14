#pragma once

#include <Arduino.h>

class LongPressButton {
public:
    typedef void (*Callback)(void*);

    LongPressButton();
    void setup(int pin, long unsigned int longPressTime, Callback onShortPress, Callback onLongPress, void* context);

    void update();
    bool isPressed();

private:
    int pin;
    Callback onShortPress;
    Callback onLongPress;
    void* context;
    int lastState;
    bool pressed;
    unsigned long buttonPressTime;
    bool isPressHandled;
    int longPressTime;
};
