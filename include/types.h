#ifndef API_TYPES_H
#define API_TYPES_H
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

enum RoomStatus {
    STATUS_FREE,
    STATUS_UPCOMING,
    STATUS_AWAITING_CONFIRMATION,
    STATUS_IN_PROGRESS,
    STATUS_ERROR
};

struct RoomStatusData {
    RoomStatus status;
    unsigned long nextMeetingStart;
    unsigned long currentMeetingEnd;
    String currentMeetingId;
    String error;
};
#endif