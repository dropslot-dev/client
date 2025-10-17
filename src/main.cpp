#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "led_controller.h"
#include "button_handler.h"
#include "api_client.h"

LEDController ledController;
ButtonHandler buttonHandler;
APIClient* apiClient = nullptr;

unsigned long lastPollTime = 0;
RoomStatus currentStatus = STATUS_ERROR;
String currentMeetingId = "";

TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;

void handleButtonPress();

void ledTask(void* parameter) {
    for(;;) {
        ledController.update();
        vTaskDelay(1000 / LED_FPS / portTICK_PERIOD_MS);
    }
}

void buttonTask(void* parameter) {
    for(;;) {
        handleButtonPress();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    ledController.setPattern(LED_WIFI_DISCONNECTED);
    
    while (WiFi.status() != WL_CONNECTED) {
        ledController.update();
        delay(50);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
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
    
    Serial.println("System ready!");
}

void handleButtonPress() {
    if (!buttonHandler.wasPressed()) {
        return;
    }
    
    switch (currentStatus) {
        case STATUS_FREE:
        if (apiClient->quickBook()) {
                Serial.println("[Button] Quick-book - OK");
            } else {
                Serial.println("[Button] Quick-book - FAIL");
            }
            break;
            
        case STATUS_AWAITING_CONFIRMATION:
        if (apiClient->confirmMeeting(currentMeetingId)) {
                Serial.println("[Button] Confirm - OK");
            } else {
                Serial.println("[Button] Confirm - FAIL");
            }
            break;
            
        case STATUS_IN_PROGRESS:
        if (apiClient->endMeeting(currentMeetingId)) {
                Serial.println("[Button] End meeting - OK");
            } else {
                Serial.println("[Button] End meeting - FAIL");
            }
            break;
            
        default:
            Serial.println("[Button] Ignored");
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
    
    if (!apiClient->getRoomStatus(statusData)) {
        Serial.println("Failed to get room status");
        Serial.print("Error: ");
        Serial.println(statusData.error);
        if (currentStatus != STATUS_ERROR) {
            ledController.setPattern(LED_ERROR_RED_BLUE);
            currentStatus = STATUS_ERROR;
        }
        return;
    }
    
    if (statusData.status != currentStatus) {
        currentStatus = statusData.status;
        currentMeetingId = statusData.currentMeetingId;
        
        switch (currentStatus) {
            case STATUS_FREE:
                Serial.println("Status: FREE");
                ledController.setPattern(LED_SOLID_GREEN);
                break;
                
            case STATUS_UPCOMING:
                Serial.println("Status: UPCOMING");
                Serial.print("Next meeting starts at: ");
                Serial.println(statusData.nextMeetingStart);
                ledController.setPattern(LED_PULSING_BLUE);
                break;
                
            case STATUS_AWAITING_CONFIRMATION:
                Serial.println("Status: AWAITING CONFIRMATION");
                Serial.print("Current meeting ends at: ");
                Serial.println(statusData.currentMeetingEnd);
                ledController.setPattern(LED_FLASHING_RED);
                break;
                
            case STATUS_IN_PROGRESS:
                Serial.println("Status: IN PROGRESS");
                Serial.print("Current meeting ends at: ");
                Serial.println(statusData.currentMeetingEnd);
                ledController.setPattern(LED_SLOW_PULSE_RED);
                break;
                
            case STATUS_ERROR:
                Serial.println("Status: ERROR");
                Serial.print("Error: ");
                Serial.println(statusData.error);
                ledController.setPattern(LED_ERROR_RED_BLUE);
                break;
        }
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Reconnecting...");
        ledController.setPattern(LED_WIFI_DISCONNECTED);
        connectToWiFi();
    }
    
    pollRoomStatus();
    delay(1000);
}
