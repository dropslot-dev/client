#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "log.h"
#include "database.h"
#include "Callback.h"
#include "types.h"
class APIClient {
public:
    APIClient(Log& rlog);
    Logger logger;

    void setup(Database &database, Signal<RoomStatusData>& roomStatusChanged);
    void loop();

    bool getRoomStatus();
    bool quickBook();
    bool confirmMeeting(const String& meetingId);
    bool endMeeting(const String& meetingId);
    void setConnected(bool connected);
    void handleButtonPress(bool pressed);

private:
    Database* database;
    String _serverUrl;
    String _roomId;
    bool _authEnabled;
    String _authToken;
    HTTPClient _http;
    Signal<RoomStatusData>* _roomStatusChanged;

    unsigned long _lastPollTime;
    bool _initialized;
    bool _connected;
    bool _buttonPressProcessed;
    RoomStatus _roomStatus = STATUS_ERROR;
    RoomStatusData _roomStatusData;

    bool makeRequest(const String& method, const String& endpoint, const String& payload, JsonDocument& response);
    void pollStatus();
};

#endif
