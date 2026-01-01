#include "wifinetwork.h"
#include <time.h>

#define WIFI_RECONNECT_INTERVAL 5000   // 5 seconds

WifiNetwork::WifiNetwork(Log& rlog) : logger(rlog, "[WIFI]") {
    wifi_connected = false;
    tries = 0;
    APstart = 0;
    lastReconnectAttempt = 0;
    apFallbackActive = false;
}

void WifiNetwork::setup(Database &database, Signal<boolean> &wifiStatusChanged){

    this->database = &database;

    this->ssid = this->database->getValueAsString(String(DB_WIFI_NAME), false);
    this->password = this->database->getValueAsString(String(DB_WIFI_PASSWORD), false);

    this->wifiStatusChanged = &wifiStatusChanged;

    uniqueBoardname = BOARD_NAME "_" + WiFi.macAddress();
    uniqueBoardname.replace(":","");
    logger << "Unique board name (hostname) is: " << uniqueBoardname;

    WiFi.onEvent(
        [this](WiFiEvent_t event, WiFiEventInfo_t info) {
            this->WiFiEvent(event);
        }
    );

    WiFi.mode(WIFI_AP_STA);
    configAP();
    setupMDNS();

    this->wifiStatusChanged->fire(false);
}

void WifiNetwork::connectWifi() {
    apFallbackActive = false;
    tries = 0;

    if (ssid.length() > 0) {
        connectToAP();
    } else {
        logger << "No SSID defined, creating AP";
        createAP();
    }
}

void WifiNetwork::disconnectWifi() {
    WiFi.disconnect();
    logger << "Wifi disconnected manually";
}

void WifiNetwork::connectToAP() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(uniqueBoardname.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
}

void WifiNetwork::createAP() {
    WiFi.mode(WIFI_AP_STA);
    configAP();
    APstart = millis();
    logger << "AP created";
}

void WifiNetwork::stopAP() {
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
    logger << "AP stopped";
}

boolean WifiNetwork::isConnected() {
    return wifi_connected;
}

void WifiNetwork::loop() {
    if (wifi_connected) {
        wifiConnectedLoop();
    } else {
        wifiDisconnectedLoop();
    }
}

void WifiNetwork::WiFiEvent(WiFiEvent_t event) {
    switch(event) {

        case SYSTEM_EVENT_AP_START:
            WiFi.softAPsetHostname(uniqueBoardname.c_str());
            logger << "AP started. SSID: " << BOARD_NAME << " AP IPv4: " << WiFi.softAPIP().toString();
            break;

        case SYSTEM_EVENT_STA_START:
            WiFi.setHostname(uniqueBoardname.c_str());
            logger << "STA started";
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            wifiOnConnect();
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            wifiOnDisconnect();
            break;

        default:
            break;
    }
}

// Called when WiFi connects successfully
void WifiNetwork::wifiOnConnect() {
    tries = 0;
    lastReconnectAttempt = 0;
    apFallbackActive = false;

    wifi_connected = true;
    stopAP();

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    wifiStatusChanged->fire(true);

    logger << "STA Connected. STA SSID: " << WiFi.SSID()
           << " STA IPv4: " << WiFi.localIP().toString()
           << ", GW: " << WiFi.gatewayIP().toString()
           << ", Mask: " << WiFi.subnetMask().toString()
           << ", DNS: " << WiFi.dnsIP().toString();
}

// Called when WiFi disconnects (does not trigger AP yet)
void WifiNetwork::wifiOnDisconnect() {
    if (wifi_connected) {
        logger << "WiFi disconnected";
    }

    wifi_connected = false;
    wifiStatusChanged->fire(false);
}

// Loop when WiFi is connected
void WifiNetwork::wifiConnectedLoop() {
    // nothing for now
}

// Loop when WiFi is disconnected
void WifiNetwork::wifiDisconnectedLoop() {

    // ---- AP fallback active ----
    if (apFallbackActive) {

        // Reboot after timeout if SSID exists
        if (ssid.length() > 0 && (millis() - APstart) > WIFI_OFF_REBOOT_TIME) {
            logger << "AP timeout reached, rebooting";
            ESP.restart();
        }

        dnsServer.processNextRequest();
        return;
    }

    // ---- Normal reconnect phase ----
    if (ssid.length() > 0 && millis() - lastReconnectAttempt > WIFI_RECONNECT_INTERVAL) {

        lastReconnectAttempt = millis();
        tries++;

        logger << "Reconnect attempt #" << String(tries);
        WiFi.disconnect(false);
        connectToAP();
    }

    // ---- Fallback to AP if max tries reached ----
    if (tries > WIFI_MAX_TRY) {
        logger << "WiFi unreachable, switching to AP mode";

        apFallbackActive = true;
        createAP();
        APstart = millis();
    }

    dnsServer.processNextRequest();
}

void WifiNetwork::setupMDNS() {
    if(!MDNS.begin(uniqueBoardname.c_str())) {
        logger << "Error starting mDNS";
    } else {
        MDNS.addService("http", "tcp", 80);
    }
}

void WifiNetwork::configAP() {
    String apName = String(BOARD_NAME) + "_" + WiFi.macAddress();
    apName.replace(":", "");

    logger << "AP name: " << apName;

    WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
    WiFi.softAP(apName.c_str(), NULL, 1, 0, 4, false);

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", AP_IP);
}
