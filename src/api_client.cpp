#include "api_client.h"
#include "auth_util.h"
#include "config.h"
#include <time.h>

extern String getTimestamp();

APIClient::APIClient(const String& serverUrl, const String& roomId, bool authEnabled)
    : _serverUrl(serverUrl), _roomId(roomId), _authEnabled(authEnabled) {
    if (_authEnabled) {
        _authToken = computeRoomIdHash(roomId);
    }
}

bool APIClient::makeRequest(const String& method, const String& endpoint, const String& payload, DynamicJsonDocument& response) {
    String url = _serverUrl + endpoint;

    
    _http.begin(url);
    _http.setTimeout(HTTP_TIMEOUT_MS);
    
    if (_authEnabled && !_authToken.isEmpty()) {
        _http.addHeader("Authorization", _authToken);
    }
    
    int httpCode = -1;
    
    unsigned long startTime = millis();
    if (method == "GET") {
        httpCode = _http.GET();
    } else if (method == "POST") {
        _http.addHeader("Content-Type", "application/json");
        httpCode = _http.POST(payload);
    }
    
    unsigned long responseTime = millis() - startTime;
    
    bool success = false;
    
    if (httpCode > 0) {
        String responsePayload = _http.getString();
        
        if (httpCode >= 200 && httpCode < 300) {
            DeserializationError error = deserializeJson(response, responsePayload);
            if (!error) {
                success = true;
                Serial.println(getTimestamp() + " [API] " + method + " " + endpoint + " - " + String(responseTime) + "ms");
            } else {
                Serial.println(getTimestamp() + " [API] " + method + " " + endpoint + " - Parse error");
            }
        } else {
            Serial.println(getTimestamp() + " [API] " + method + " " + endpoint + " - HTTP " + String(httpCode) + " (" + String(responseTime) + "ms)");
        }
    } else {
        Serial.println(getTimestamp() + " [API] " + method + " " + endpoint + " - Request failed");
    }
    
    _http.end();
    return success;
}

bool APIClient::getRoomStatus(RoomStatusData& data) {
    DynamicJsonDocument doc(1024);
    String endpoint = "/api/v1/rooms/" + _roomId + "/status";
    
    if (!makeRequest("GET", endpoint, "", doc)) {
        data.status = STATUS_ERROR;
        data.error = "Failed to get status";
        return false;
    }
    
    String stateStr = doc["state"].as<String>();
    
    if (stateStr == "free") {
        data.status = STATUS_FREE;
    } else if (stateStr == "upcoming") {
        data.status = STATUS_UPCOMING;
        if (doc["current_meeting"]["start"]) {
            data.nextMeetingStart = doc["current_meeting"]["start"].as<unsigned long>();
        }
    } else if (stateStr == "confirmation_required") {
        data.status = STATUS_AWAITING_CONFIRMATION;
        data.currentMeetingId = doc["current_meeting"]["id"].as<String>();
        if (doc["current_meeting"]["end"]) {
            data.currentMeetingEnd = doc["current_meeting"]["end"].as<unsigned long>();
        }
    } else if (stateStr == "in_progress") {
        data.status = STATUS_IN_PROGRESS;
        data.currentMeetingId = doc["current_meeting"]["id"].as<String>();
        if (doc["current_meeting"]["end"]) {
            data.currentMeetingEnd = doc["current_meeting"]["end"].as<unsigned long>();
        }
    } else {
        data.status = STATUS_ERROR;
        data.error = "Unknown state: " + stateStr;
        return false;
    }
    
    return true;
}

bool APIClient::quickBook() {
    DynamicJsonDocument doc(512);
    String endpoint = "/api/v1/rooms/" + _roomId + "/quick-book";
    
    return makeRequest("POST", endpoint, "", doc);
}

bool APIClient::confirmMeeting(const String& meetingId) {
    DynamicJsonDocument doc(512);
    String endpoint = "/api/v1/rooms/" + _roomId + "/confirm";
    
    DynamicJsonDocument payload(128);
    payload["meeting_id"] = meetingId;
    
    String payloadStr;
    serializeJson(payload, payloadStr);
    
    return makeRequest("POST", endpoint, payloadStr, doc);
}

bool APIClient::endMeeting(const String& meetingId) {
    DynamicJsonDocument doc(512);
    String endpoint = "/api/v1/rooms/" + _roomId + "/end-meeting";
    
    DynamicJsonDocument payload(128);
    payload["meeting_id"] = meetingId;
    
    String payloadStr;
    serializeJson(payload, payloadStr);
    
    return makeRequest("POST", endpoint, payloadStr, doc);
}
