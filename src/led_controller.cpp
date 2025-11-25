#include "led_controller.h"


LEDController::LEDController(Log& rlog) : logger(rlog, "[LED]") {
}

void LEDController::setup() {
    _currentPattern = LED_OFF;
    _lastUpdate = 0;
    _pulsePhase = 0;
    _flashState = false;
    _pulsing = false;
    _pulseStartTime = 0;

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
        _pulsing = false;
    }
}

void LEDController::pulse() {
    _pulsing = true;
    _pulseStartTime = millis();
}

void LEDController::loop() {
    unsigned long now = millis();
    unsigned long elapsed = now - _lastUpdate;

    if (elapsed < (1000 / LED_FPS)) {
        return;
    }

    _lastUpdate = now;

    applyPattern();
}

void LEDController::setRoomStatus(RoomStatusData status) {
    if (status.status != _roomStatus.status) {
        _roomStatus = status;
    }

    updateLedPattern();
}

void LEDController::updateLedPattern() {
        switch (_roomStatus.status) {
            case STATUS_FREE:
                logger << getTimestamp() + " Status: FREE";
                setPattern(LED_SOLID_GREEN);
                break;

            case STATUS_UPCOMING:
                logger << getTimestamp() + " Status: UPCOMING";
                setPattern(LED_PULSING_BLUE);
                break;

            case STATUS_AWAITING_CONFIRMATION:
                logger << getTimestamp() + " Status: AWAITING CONFIRMATION";
                setPattern(LED_FLASHING_RED);
                break;

            case STATUS_IN_PROGRESS:
                logger << getTimestamp() + " Status: IN PROGRESS";
                setPattern(LED_SOLID_RED);
                break;

            case STATUS_ERROR:
                logger << getTimestamp() + " Status: ERROR";
                logger << getTimestamp() + " Error: " + _roomStatus.error;
                setPattern(LED_ERROR_WHITE);
                break;
        }
}

void LEDController::applyPattern() {
    unsigned long now = millis();

    switch (_currentPattern) {
        case LED_OFF:
            turnOff();
            break;

        case LED_SOLID_GREEN: {
            if (_pulsing) {
                float phase = (now % 1000) / 1000.0;
                int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
                setPWM(0, brightness, 0);
            } else {
                setPWM(0, 255, 0);
            }
            break;
        }

        case LED_PULSING_BLUE: {
            float phase = (now % 2000) / 2000.0;
            int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
            setPWM(0, 0, brightness);
            break;
        }

        case LED_FLASHING_RED: {
            if (_pulsing) {
                float phase = (now % 1000) / 1000.0;
                int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
                setPWM(brightness, 0, 0);
            } else if ((now % 500) < 250) {
                setPWM(255, 0, 0);
            } else {
                turnOff();
            }
            break;
        }

        case LED_SOLID_RED: {
            if (_pulsing) {
                float phase = (now % 1000) / 1000.0;
                int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
                setPWM(brightness, 0, 0);
            } else {
                setPWM(255, 0, 0);
            }
            break;
        }

        case LED_ERROR_WHITE: {
            float phase = (now % 2000) / 2000.0;
            int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
            setPWM(brightness, brightness, brightness);
            break;
        }

        case LED_WIFI_DISCONNECTED: {
            float phase = (now % 2000) / 2000.0;
            int brightness = (sin(phase * 2 * PI) + 1) * 127.5;
            setPWM(0, brightness, brightness);
            break;
        }
    }
}