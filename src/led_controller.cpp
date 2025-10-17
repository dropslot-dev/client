#include "led_controller.h"
#include "config.h"

LEDController::LEDController() 
    : _currentPattern(LED_OFF), _lastUpdate(0), _pulsePhase(0), _flashState(false) {
}

void LEDController::begin() {
    ledcSetup(RED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(GREEN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(BLUE_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    
    ledcAttachPin(RED_PIN, RED_CHANNEL);
    ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);
    ledcAttachPin(BLUE_PIN, BLUE_CHANNEL);
    
    turnOff();
}

void LEDController::setPWM(int r, int g, int b) {
    ledcWrite(RED_CHANNEL, 255 - r);
    ledcWrite(GREEN_CHANNEL, 255 - g);
    ledcWrite(BLUE_CHANNEL, 255 - b);
}

void LEDController::turnOff() {
    setPWM(0, 0, 0);
}

void LEDController::setPattern(LEDPattern pattern) {
    if (_currentPattern != pattern) {
        _currentPattern = pattern;
        _pulsePhase = 0;
        _flashState = false;
        _lastUpdate = millis();
    }
}

void LEDController::update() {
    unsigned long now = millis();
    unsigned long elapsed = now - _lastUpdate;
    
    if (elapsed < (1000 / LED_FPS)) {
        return;
    }
    
    _lastUpdate = now;
    
    switch (_currentPattern) {
        case LED_OFF:
            turnOff();
            break;
            
        case LED_SOLID_GREEN:
            setPWM(0, 255, 0);
            break;
            
        case LED_PULSING_BLUE: {
            float phase = (now % 2000) / 2000.0;
            int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
            setPWM(0, 0, brightness);
            break;
        }
        
        case LED_FLASHING_RED: {
            if ((now % 500) < 250) {
                setPWM(255, 0, 0);
            } else {
                turnOff();
            }
            break;
        }
        
        case LED_SLOW_PULSE_RED: {
            float phase = (now % 3000) / 3000.0;
            int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
            setPWM(brightness, 0, 0);
            break;
        }
        
        case LED_ERROR_RED_BLUE: {
            if ((now % 2000) < 1000) {
                setPWM(255, 0, 0);
            } else {
                setPWM(0, 0, 255);
            }
            break;
        }
        
        case LED_WIFI_DISCONNECTED: {
            if ((now % 1000) < 500) {
                setPWM(0, 255, 255);
            } else {
                setPWM(255, 0, 255);
            }
            break;
        }
    }
}
