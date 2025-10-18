#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

enum LEDPattern {
    LED_OFF,
    LED_SOLID_GREEN,
    LED_PULSING_BLUE,
    LED_FLASHING_RED,
    LED_SOLID_RED,
    LED_ERROR_WHITE,
    LED_WIFI_DISCONNECTED
};

class LEDController {
public:
    LEDController();
    
    void begin();
    void setPattern(LEDPattern pattern);
    void update();
    void pulse();

private:
    LEDPattern _currentPattern;
    unsigned long _lastUpdate;
    float _pulsePhase;
    bool _flashState;
    bool _pulsing;
    unsigned long _pulseStartTime;
    
    void setPWM(int r, int g, int b);
    void turnOff();
};

#endif
