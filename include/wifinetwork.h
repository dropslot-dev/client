#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <WiFi.h>
#include "WiFiUdp.h"
#include <Callback.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "log.h"
#include "database.h"

class WifiNetwork {

    public:

        Logger logger;
        Signal<boolean>* wifiStatusChanged;
        bool wifi_connected;
        DNSServer dnsServer;
        Database* database;

        String ssid;
        String password;

        int tries;
        int APstart;
        int lastReconnectAttempt;
        bool apFallbackActive;
        String uniqueBoardname;

        WifiNetwork(Log& rlog);
        void setup(Database &database, Signal<boolean> &wifiStatusChanged);
        void loop();
        void connectWifi();
        void disconnectWifi();
        void connectToAP();
        void createAP();
        void stopAP();
        boolean isConnected();

        #define WIFI_RECONNECT_INTERVAL 5000  // 5 seconds


    private:

        void WiFiEvent(WiFiEvent_t event);
        // when wifi connects
        void wifiOnConnect();
        // when wifi disconnects
        void wifiOnDisconnect();
        // while wifi is connected
        void wifiConnectedLoop();
        // while wifi is not connected
        void wifiDisconnectedLoop();
        void setupMDNS();
        void configAP();
};

#endif
