#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "log.h"
#include "Callback.h"

class ButtonHandler {
public:
    ButtonHandler(Log& rlog);
    void setup(Signal<boolean>& buttonPressed);
    void loop();

private:
    Logger logger;
    Signal<boolean>* _buttonState;
    unsigned long _lastDebounceTime;
    bool _pressStatus;
    bool _lastReading;
    bool _stableState;
};

#endif
