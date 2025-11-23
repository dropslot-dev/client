#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "log.h"
#include "database.h"

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

class APIClient {
public:
    APIClient(Log& rlog);
    Logger logger;
    Database* database;

    void setup(Database &database);
    void loop();

    bool getRoomStatus(RoomStatusData& data);
    bool quickBook();
    bool confirmMeeting(const String& meetingId);
    bool endMeeting(const String& meetingId);

private:
    String _serverUrl;
    String _roomId;
    bool _authEnabled;
    String _authToken;
    HTTPClient _http;

    unsigned long _lastPollTime;
    bool _initialized;

    bool makeRequest(const String& method, const String& endpoint, const String& payload, JsonDocument& response);
    void pollStatus();
};

#endif
