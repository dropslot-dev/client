#ifndef CONFIG_H
#define CONFIG_H

#define MAJOR_VERSION 1.1
#define BOARD_NAME "DropSlotClient"
#define EEPROM_SIZE 1024
#define COMMAND_CONFIG "config"

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define SERIAL_SPEED 115200

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
#define WIFI_MAX_TRY 5
#define WIFI_OFF_REBOOT_TIME 5*60*1000 // If SSID is configured, but still AP is created restart after this time. Ex: Wifi router is gone -> device goes to AP mode. Without this it never tries to connect again. Just after a reboot
#define AP_IP {192, 168, 4, 1} // Change together with the string version
#define AP_IP_STRING "192.168.4.1" // Change together with the object version
#define AP_NETMASK {255, 255, 255, 0}

#define LED_FPS 30

// ERRORS
#define ERROR_NO_ERROR 0
#define ERROR_UNKNOWN 1
#define ERROR_WIFI 2

// DATABASE PROPERTIES
#define DB_WIFI_NAME "ssid"
#define DB_WIFI_PASSWORD "pw"
#define DB_VERSION "version"
#define DB_DEVICE_ID "deviceid"


#endif
