#include "button_handler.h"
#include "config.h"

ButtonHandler::ButtonHandler(Log& rlog): logger(rlog, "[BUTTON]") {}

void ButtonHandler::setup(Signal<boolean>& buttonState) {
    _buttonState = &buttonState;

    _lastReading = HIGH;
    _stableState = HIGH;
    _lastDebounceTime = 0;

    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void ButtonHandler::loop() {
    bool reading = digitalRead(BUTTON_PIN);
    unsigned long now = millis();

    // reset the debounce timer on state change
    if (reading != _lastReading) {
        _lastDebounceTime = now;
    }

    _lastReading = reading;

    if ((now - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {

        if (reading != _stableState) {
            _stableState = reading;

            if (_stableState == LOW) {
                // Pressed
                buttonState->fire(true);
            } else {
                // Released
                buttonState->fire(false);
            }
        }
    }
}
