#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "Callback.h"
#include "config.h"
#include "log.h"
#include "database.h"
#include "led_controller.h"
#include "button_handler.h"
#include "api_client.h"
#include "utilities.h"
#include "wifinetwork.h"
#include "webservice.h"

LEDController ledController;
ButtonHandler buttonHandler;
Log rlog;
Database database(rlog);
WifiNetwork wifi(rlog);
Webservice webservice(rlog);
APIClient apiClient(rlog);

Logger logger(rlog, "[MAIN]");

// This signal will be emitted when we process characters
// https://github.com/tomstewart89/Callback
Signal<boolean> wifiStatusChanged;

unsigned long lastPollTime = 0;
RoomStatus currentStatus = STATUS_ERROR;
String currentMeetingId = "";

TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;

void handleButtonPress();

void ledTask(void* parameter) {
    for(;;) {
        ledController.loop();
        vTaskDelay(1000 / LED_FPS / portTICK_PERIOD_MS);
    }
}

void buttonTask(void* parameter) {
    for(;;) {
        handleButtonPress();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*
void connectToWiFi() {
    Serial.println(getTimestamp() + " Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    ledController.setPattern(LED_WIFI_DISCONNECTED);

    while (WiFi.status() != WL_CONNECTED) {
        ledController.update();
        signed long wifiProgress = millis();
        if (millis() - wifiProgress < 50) {
            Serial.print(".");
        }
    }

    Serial.println("");
    Serial.println(getTimestamp() + " WiFi connected!");
    Serial.print(getTimestamp() + " IP address: ");
    Serial.println(WiFi.localIP());

    configTime(0, 0, "pool.ntp.org");
}
*/

void setup() {
    setChipInfo();
    rlog.setup();

    logger << "\n=== Dropslot Room Controller ===";

    database.setup();
    wifi.setup(database, wifiStatusChanged);

    // Must be after Wifi setup
    webservice.setup(database);

    // Connect to WiFi
    wifi.connectWifi();

    apiClient.setup(database);

    ledController.setup();
    buttonHandler.setup();



    /*
    signed long serialInitTime = millis();
    while (millis() - serialInitTime < 1000) {
        // Wait up to (non-blocking) 1 second for Serial to initialize
    }

    Serial.println("\n=== Dropslot Room Controller ===");
    Serial.print("Room ID: ");
    Serial.println(ROOM_ID);

    ledController.begin();
    buttonHandler.begin();

    connectToWiFi();

    apiClient = new APIClient(SERVER_URL, ROOM_ID, AUTH_ENABLED);

    xTaskCreatePinnedToCore(
        ledTask,
        "LED Task",
        2048,
        NULL,
        1,
        &ledTaskHandle,
        0
    );

    xTaskCreatePinnedToCore(
        buttonTask,
        "Button Task",
        4096,
        NULL,
        2,
        &buttonTaskHandle,
        1
    );
*/

}

void handleButtonPress() {
    if (!buttonHandler.wasPressed()) {
        return;
    }

    ledController.pulse();

    switch (currentStatus) {
        case STATUS_FREE:
        if (apiClient.quickBook()) {
                Serial.println(getTimestamp() + " [Button] Quick-book - OK");
            } else {
                Serial.println(getTimestamp() + " [Button] Quick-book - FAIL");
            }
            break;

        case STATUS_AWAITING_CONFIRMATION:
        if (apiClient.confirmMeeting(currentMeetingId)) {
                Serial.println(getTimestamp() + " [Button] Confirm - OK");
            } else {
                Serial.println(getTimestamp() + " [Button] Confirm - FAIL");
            }
            break;

        case STATUS_IN_PROGRESS:
        if (apiClient.endMeeting(currentMeetingId)) {
                Serial.println(getTimestamp() + " [Button] End meeting - OK");
            } else {
                Serial.println(getTimestamp() + " [Button] End meeting - FAIL");
            }
            break;

        default:
            Serial.println(getTimestamp() + " [Button] Ignored");
            break;
    }

    lastPollTime = 0;
}

void pollRoomStatus() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    unsigned long now = millis();

    if (now - lastPollTime < POLL_INTERVAL_MS) {
        return;
    }

    lastPollTime = now;

    RoomStatusData statusData;

    if (!apiClient.getRoomStatus(statusData)) {
        Serial.print(getTimestamp() + " Failed to get room status: ");
        Serial.println(statusData.error);
        if (currentStatus != STATUS_ERROR) {
            ledController.setPattern(LED_ERROR_WHITE);
            currentStatus = STATUS_ERROR;
        }
        return;
    }

    if (statusData.status != currentStatus) {
        currentStatus = statusData.status;
        currentMeetingId = statusData.currentMeetingId;

        switch (currentStatus) {
            case STATUS_FREE:
                Serial.println(getTimestamp() + " Status: FREE");
                ledController.setPattern(LED_SOLID_GREEN);
                break;

            case STATUS_UPCOMING:
                Serial.println(getTimestamp() + " Status: UPCOMING");
                ledController.setPattern(LED_PULSING_BLUE);
                break;

            case STATUS_AWAITING_CONFIRMATION:
                Serial.println(getTimestamp() + " Status: AWAITING CONFIRMATION");
                ledController.setPattern(LED_FLASHING_RED);
                break;

            case STATUS_IN_PROGRESS:
                Serial.println(getTimestamp() + " Status: IN PROGRESS");
                ledController.setPattern(LED_SOLID_RED);
                break;

            case STATUS_ERROR:
                Serial.println(getTimestamp() + " Status: ERROR");
                Serial.print(getTimestamp() + " Error: ");
                Serial.println(statusData.error);
                ledController.setPattern(LED_ERROR_WHITE);
                break;
        }
    }
}

void loop() {
    wifi.loop();
    webservice.loop();
    apiClient.loop();
    buttonHandler.loop();
    ledController.loop();

    // old logic, needs to be refactored
    pollRoomStatus();
}
