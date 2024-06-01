#pragma once

#include "Button.h"

class LongPressButton : public Button {
public:
    LongPressButton();
    void setup(int pin, unsigned long longPressTime, Callback onShortPress, Callback onLongPress, void* context);
    void update() override;

private:
    Callback onShortPress;
    Callback onLongPress;
    unsigned long buttonPressTime;
    bool isPressHandled;
    unsigned long longPressTime;
};
