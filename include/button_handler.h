#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

class ButtonHandler {
public:
    ButtonHandler();

    void setup();
    void loop();
    bool wasPressed(); // This must be refactored, move the logic to loop

private:
    bool _lastState;
    unsigned long _lastDebounceTime;
    bool _buttonPressed;
};

#endif
