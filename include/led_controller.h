#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include "log.h"
#include "types.h"
#include "config.h"
#include "utilities.h"

class LEDController {
public:
    LEDController(Log& rlog);

    void setup();
    void setPattern(LEDPattern pattern);
    void loop();
    void pulse();
    void setRoomStatus(RoomStatusData status);

private:
    LEDPattern _currentPattern;
    Logger logger;
    unsigned long _lastUpdate;
    float _pulsePhase;
    bool _flashState;
    bool _pulsing;
    RoomStatusData _roomStatus;
    unsigned long _pulseStartTime;

    void setPWM(int r, int g, int b);
    void turnOff();
    void updateLedPattern();
    void applyPattern();
};

#endif
