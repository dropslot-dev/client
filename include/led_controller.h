#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

enum LEDPattern {
    LED_OFF,
    LED_SOLID_GREEN,
    LED_PULSING_BLUE,
    LED_FLASHING_RED,
    LED_SLOW_PULSE_RED,
    LED_ERROR_RED_BLUE,
    LED_WIFI_DISCONNECTED
};

class LEDController {
public:
    LEDController();
    
    void begin();
    void setPattern(LEDPattern pattern);
    void update();

private:
    LEDPattern _currentPattern;
    unsigned long _lastUpdate;
    float _pulsePhase;
    bool _flashState;
    
    void setPWM(int r, int g, int b);
    void turnOff();
};

#endif
