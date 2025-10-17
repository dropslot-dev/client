#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

class ButtonHandler {
public:
    ButtonHandler();
    
    void begin();
    bool wasPressed();

private:
    bool _lastState;
    unsigned long _lastDebounceTime;
    bool _buttonPressed;
};

#endif
