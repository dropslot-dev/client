#include "led_controller.h"

LEDController::LEDController(Log& rlog) : logger(rlog, "[LED]") {
}

void LEDController::setup() {
    _currentPattern = LED_OFF;
    _lastUpdate = 0;
    _pulsePhase = 0;
    _flashState = false;
    _pulsing = true;
    _pulseStartTime = 0;
    _connected = false;

    ledcSetup(RED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(GREEN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(BLUE_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

    ledcAttachPin(RED_PIN, RED_CHANNEL);
    ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);
    ledcAttachPin(BLUE_PIN, BLUE_CHANNEL);

    turnOff();

    logger << getTimestamp() + " LED Controller initialized.";
}

void LEDController::setPWM(int r, int g, int b) {
    // ESP32: common anode LED feltételezve
    ledcWrite(RED_CHANNEL, 255 - r);
    ledcWrite(GREEN_CHANNEL, 255 - g);
    ledcWrite(BLUE_CHANNEL, 255 - b);
}

void LEDController::turnOff() {
    setPattern(LED_OFF);
    setPWM(0, 0, 0);
}

void LEDController::setPattern(LEDPattern pattern) {
    if (_currentPattern != pattern) {
        _currentPattern = pattern;
        _pulsePhase = 0;        // pulzus kezdő fázis
        _flashState = false;
        _lastUpdate = millis();
        _pulsing = false;
    }
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
    if (!_connected) return;

    if (status.status != _roomStatus.status) {
        _roomStatus.previousStatus = _roomStatus.status;
        _roomStatus = status;
    }

    updateLedPattern();
}

void LEDController::setConnected(bool connected) {
    this->_connected = connected;
    if (!connected) {
        setPattern(LED_WIFI_DISCONNECTED);
        updateLedPattern();
    }
}

void LEDController::updateLedPattern() {
    if (!_connected) return;

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

    // Fázis frissítése minden frame-ben
    const float pulseSpeed = 0.08; // pulzus sebessége (nagyobb → gyorsabb)
    _pulsePhase += pulseSpeed;
    if (_pulsePhase > 2 * PI) _pulsePhase -= 2 * PI;

    // Fényerő kiszámítása
    int pulse = (sin(_pulsePhase) * 0.5 + 0.5) * 255;

    switch (_currentPattern) {
        case LED_OFF:
            setPWM(0, 0, 0);
            break;

        case LED_SOLID_GREEN:
            if (_pulsing) {
                setPWM(0, pulse, 0);
            } else {
                setPWM(0, 255, 0);
            }
            break;

        case LED_PULSING_BLUE:
            setPWM(0, 0, pulse);
            break;

        case LED_FLASHING_RED: {
            if (_pulsing) {
                // finom pulzus
                setPWM(pulse, 0, 0);
            } else {
                // lágy villogás 1500 ms periódus
                unsigned long period = 1500;
                float phase = (now % period) / (float)period;
                int brightness = (sin(phase * 2 * PI) * 0.5 + 0.5) * 255;
                setPWM(brightness, 0, 0);
            }
            break;
        }

        case LED_SOLID_RED:
            if (_pulsing) {
                setPWM(pulse, 0, 0);
            } else {
                setPWM(255, 0, 0);
            }
            break;

        case LED_ERROR_WHITE:
            setPWM(pulse, pulse, pulse);
            break;

        case LED_WIFI_DISCONNECTED:
            // kicsit halványabb kék-zöld pulzus
            setPWM(30, pulse, pulse);
            break;
    }
}
