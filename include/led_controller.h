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
    void setRoomStatus(RoomStatusData status);
    void setConnected(bool connected);

private:
    LEDPattern _currentPattern;
    Logger logger;
    unsigned long _lastUpdate;
    float _pulsePhase;
    bool _flashState;
    bool _pulsing;
    bool _connected;
    RoomStatusData _roomStatus;
    unsigned long _pulseStartTime;

    void setPWM(int r, int g, int b);
    void turnOff();
    void updateLedPattern();
    void applyPattern();
};

#endif
