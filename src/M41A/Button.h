#pragma once

#include <Arduino.h>

class Button {
public:
    typedef void (*Callback)(void*);

    Button();
    void setup(int pin, Callback onPress, Callback onRelease, void* context);

    void update();
    bool isPressed();

private:
    int pin;
    Callback onPress;
    Callback onRelease;
    void* context;
    int lastState;
    bool pressed;
};
