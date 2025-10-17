#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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
    APIClient(const String& serverUrl, const String& roomId, bool authEnabled);
    
    bool getRoomStatus(RoomStatusData& data);
    bool quickBook(int durationMinutes);
    bool confirmMeeting(const String& meetingId);
    bool endMeeting(const String& meetingId);

private:
    String _serverUrl;
    String _roomId;
    bool _authEnabled;
    String _authToken;
    HTTPClient _http;
    
    bool makeRequest(const String& method, const String& endpoint, const String& payload, DynamicJsonDocument& response);
};

#endif
