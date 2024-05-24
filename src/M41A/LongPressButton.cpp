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
            buttonPressTime = millis(); // Uložení času stisku
            isPressHandled = false; // Resetování flagu pro zpracování akce
        } else { // Talčítko bylo uvolněno
            if (!isPressHandled &&onShortPress) {
              isPressHandled = true; // Nastavení flagu, že akce byla zpracována
              onShortPress(context);// Kontrola, zda nebyla akce již zpracována
            }
        }
        lastState = currentState;
    } else if (currentState == LOW && !isPressHandled) {
        // Kontrola, zda doba stisku přesáhla limit pro dlouhý stisk
        if (millis() - buttonPressTime >= longPressTime && onLongPress) {
            isPressHandled = true; // Nastavení flagu, že akce byla zpracována
            onLongPress(context); // Zpracování dlouhého stisku
        }
    }
}

bool LongPressButton::isPressed() {
    return pressed;
}
