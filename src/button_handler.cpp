#include "button_handler.h"
#include "config.h"

ButtonHandler::ButtonHandler() 
    : _lastState(HIGH), _lastDebounceTime(0), _buttonPressed(false) {
}

void ButtonHandler::begin() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

bool ButtonHandler::wasPressed() {
    bool currentState = digitalRead(BUTTON_PIN);
    unsigned long now = millis();
    
    if (currentState != _lastState) {
        _lastDebounceTime = now;
        Serial.print("[Button] State change: ");
        Serial.println(currentState == LOW ? "PRESSED" : "RELEASED");
    }
    
    _lastState = currentState;
    
    bool pressed = false;
    
    if ((now - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (currentState == LOW && !_buttonPressed) {
            pressed = true;
            _buttonPressed = true;
            Serial.println("[Button] Button press detected!");
        } else if (currentState == HIGH) {
            _buttonPressed = false;
        }
    }
    
    return pressed;
}
