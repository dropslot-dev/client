#ifndef CONFIG_H
#define CONFIG_H

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define SERVER_URL "http://192.168.13.133:8080"
#define ROOM_ID "001"

#define POLL_INTERVAL_MS 5000

#define AUTH_ENABLED true

#define RED_PIN     25
#define GREEN_PIN   26
#define BLUE_PIN    27
#define BUTTON_PIN  4

#define RED_CHANNEL   0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL  2
#define PWM_FREQ      5000
#define PWM_RESOLUTION 8

#define DEBOUNCE_DELAY_MS 50
#define HTTP_TIMEOUT_MS 10000
#define WIFI_RETRY_DELAY_MS 2000

#define LED_FPS 30

#endif
