#include "button_handler.h"
#include "config.h"

ButtonHandler::ButtonHandler()
    : _lastState(HIGH), _lastDebounceTime(0), _buttonPressed(false) {
}

void ButtonHandler::setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void ButtonHandler::loop() {
    // This method can be expanded if needed
}

bool ButtonHandler::wasPressed() {
    bool currentState = digitalRead(BUTTON_PIN);
    unsigned long now = millis();

    if (currentState != _lastState) {
        _lastDebounceTime = now;
    }

    _lastState = currentState;

    bool pressed = false;

    if ((now - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (currentState == LOW && !_buttonPressed) {
            pressed = true;
            _buttonPressed = true;
        } else if (currentState == HIGH) {
            _buttonPressed = false;
        }
    }

    return pressed;
}
