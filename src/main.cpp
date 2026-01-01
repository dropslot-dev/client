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

Log rlog;
LEDController ledController(rlog);
ButtonHandler buttonHandler(rlog);
Database database(rlog);
WifiNetwork wifi(rlog);
Webservice webservice(rlog);
APIClient apiClient(rlog);

Logger logger(rlog, "[MAIN]");

// This signal will be emitted when we process characters
// https://github.com/tomstewart89/Callback
Signal<boolean> wifiStatusChanged;
Signal<RoomStatusData> roomStatusChanged;
Signal<boolean> buttonPressed;


void setup() {
    setChipInfo();
    rlog.setup();
    delay(500);

    logger << "\n=== Dropslot Room Controller ===";

    // Wifi status changed
    MethodSlot<APIClient, boolean> wifiChangedForApiClient(&apiClient,&APIClient::setConnected);
    wifiStatusChanged.attach(wifiChangedForApiClient);

    MethodSlot<LEDController, boolean> wifiChangedForLed(&ledController,&LEDController::setConnected);
    wifiStatusChanged.attach(wifiChangedForLed);

    // Room status changed
    MethodSlot<LEDController, RoomStatusData> roomStatusChangedForLed(&ledController,&LEDController::setRoomStatus);
    roomStatusChanged.attach(roomStatusChangedForLed);

    // Button pressed event
    MethodSlot<APIClient, boolean> buttonPressedForApiClient(&apiClient,&APIClient::handleButtonPress);
    buttonPressed.attach(buttonPressedForApiClient);

    database.setup();
    ledController.setup();
    buttonHandler.setup(buttonPressed);
    wifi.setup(database, wifiStatusChanged);
    // Must be after Wifi setup
    webservice.setup(database);

    wifi.connectWifi();

    apiClient.setup(database, roomStatusChanged);
}

void loop() {
    wifi.loop();
    webservice.loop();
    apiClient.loop();
    buttonHandler.loop();
    ledController.loop();
}
