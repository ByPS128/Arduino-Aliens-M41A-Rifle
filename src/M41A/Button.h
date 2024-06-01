#pragma once

#include <Arduino.h>

class Button {
public:
    typedef void (*Callback)(void*);

    Button();
    void setup(int pin, Callback onPress, Callback onRelease, void* context);
    virtual void update();
    bool isPressed();

protected:
    int pin;
    Callback onPress;
    Callback onRelease;
    void* context;
    int lastState;
    bool pressed;
};
