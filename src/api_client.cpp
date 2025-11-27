#include "api_client.h"
#include "auth_util.h"
#include "config.h"
#include <time.h>

extern String getTimestamp();

APIClient::APIClient(Log& rlog) : logger(rlog, "[API]") {
    logger << getTimestamp() << "APIClient constructor called";
}

void APIClient::setup(Database &database, Signal<RoomStatusData>& roomStatusChanged) {

    this -> database = &database;
    this -> _roomStatusChanged = &roomStatusChanged;

    this -> _serverUrl = this -> database -> getValueAsString(String(DB_SERVER_URL), false);
    this -> _roomId = this -> database -> getValueAsString(String(DB_ROOM_ID), false);
    this -> _authEnabled = this -> database -> getValueAsBoolean(String(DB_AUTH_ENABLED), false, false);
    this -> _pollIntervalMs = this -> database -> getValueAsInt(String(DB_POLL_INTERVAL), false, DEFAULT_POLL_INTERVAL_MS);

    if (_authEnabled) {
        _authToken = computeRoomIdHash(_roomId);
    }

    logger << getTimestamp() << "Initializing API client for room: " + _roomId;
    logger << getTimestamp() << "Server URL: " + _serverUrl;
    logger << getTimestamp() << "Auth enabled: " + String(_authEnabled);
    logger << getTimestamp() << "Auth token: " + _authToken;

    _initialized = true;
    _lastPollTime = 0;
}

void APIClient::loop() {
    if (!_initialized || !_connected) {
        return;
    }

    pollStatus();
}

void APIClient::setConnected(bool connected) {
    _connected = connected;
    if (connected) {
        logger << getTimestamp() + " [API] Connected to network, resetting last poll time.";
        _lastPollTime = 0;
    }
}

void APIClient::handleButtonPress(bool pressed) {
    if (!pressed) {
        _buttonPressProcessed = false;
    }
    // Make sure we only process the press once per press event
    if (pressed && !_buttonPressProcessed) {
        _buttonPressProcessed = true;
        switch (_roomStatus) {
            case STATUS_FREE:
                if (quickBook()) {
                        logger << getTimestamp() + " [Button] Quick-book - OK";
                    } else {
                        logger << getTimestamp() + " [Button] Quick-book - FAIL";
                    }
                    break;

            case STATUS_AWAITING_CONFIRMATION:
                if (confirmMeeting(_roomStatusData.currentMeetingId)) {
                        logger << getTimestamp() + " [Button] Confirm - OK";
                    } else {
                        logger << getTimestamp() + " [Button] Confirm - FAIL";
                    }
                    break;

            case STATUS_IN_PROGRESS:
                if (endMeeting(_roomStatusData.currentMeetingId)) {
                        logger << getTimestamp() + " [Button] End meeting - OK";
                    } else {
                        logger << getTimestamp() + " [Button] End meeting - FAIL";
                    }
                    break;

            default:
                logger << getTimestamp() + " [Button] Ignored";
                break;
        }
    }
}

void APIClient::pollStatus() {
    unsigned long now = millis();

    if (now - _lastPollTime < _pollIntervalMs) {
        return;
    }
    _lastPollTime = now;

    if (!getRoomStatus()) {
        Serial.print(getTimestamp() + " Failed to get room status: ");
        Serial.println(_roomStatusData.error);
        if (_roomStatus != STATUS_ERROR) {
            _roomStatus = STATUS_ERROR;
        }

    } else {
        if (_roomStatus != _roomStatusData.status) {
            _roomStatus = _roomStatusData.status;
        }
    }

    // Notify the subscribers about the status change
    _roomStatusChanged->fire(_roomStatusData);
}

bool APIClient::makeRequest(const String& method, const String& endpoint, const String& payload, JsonDocument& response) {
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
                logger << getTimestamp() + " [API] " + method + " " + endpoint + " - " + String(responseTime) + "ms";
            } else {
                logger << getTimestamp() + " [API] " + method + " " + endpoint + " - Parse error";
            }
        } else {
            logger << getTimestamp() + " [API] " + method + " " + endpoint + " - HTTP " + String(httpCode) + " (" + String(responseTime) + "ms)";
        }
    } else {
        logger << getTimestamp() + " [API] " + method + " " + endpoint + " - Request failed";
    }

    _http.end();
    return success;
}

bool APIClient::getRoomStatus() {
    JsonDocument doc;
    String endpoint = "/api/v1/rooms/" + _roomId + "/status";

    if (!makeRequest("GET", endpoint, "", doc)) {
        _roomStatusData.status = STATUS_ERROR;
        _roomStatusData.error = "Failed to get status";
        return false;
    }

    String stateStr = doc["state"].as<String>();

    if (stateStr == "free") {
        _roomStatusData.status = STATUS_FREE;
    } else if (stateStr == "upcoming") {
        _roomStatusData.status = STATUS_UPCOMING;
        if (doc["current_meeting"]["start"]) {
            _roomStatusData.nextMeetingStart = doc["current_meeting"]["start"].as<unsigned long>();
        }
    } else if (stateStr == "confirmation_required") {
        _roomStatusData.status = STATUS_AWAITING_CONFIRMATION;
        _roomStatusData.currentMeetingId = doc["current_meeting"]["id"].as<String>();
        if (doc["current_meeting"]["end"]) {
            _roomStatusData.currentMeetingEnd = doc["current_meeting"]["end"].as<unsigned long>();
        }
    } else if (stateStr == "in_progress") {
        _roomStatusData.status = STATUS_IN_PROGRESS;
        _roomStatusData.currentMeetingId = doc["current_meeting"]["id"].as<String>();
        if (doc["current_meeting"]["end"]) {
            _roomStatusData.currentMeetingEnd = doc["current_meeting"]["end"].as<unsigned long>();
        }
    } else {
        _roomStatusData.status = STATUS_ERROR;
        _roomStatusData.error = "Unknown state: " + stateStr;
        return false;
    }

    return true;
}

bool APIClient::quickBook() {
    JsonDocument doc;
    String endpoint = "/api/v1/rooms/" + _roomId + "/quick-book";

    return makeRequest("POST", endpoint, "", doc);
}

bool APIClient::confirmMeeting(const String& meetingId) {
    JsonDocument doc;
    String endpoint = "/api/v1/rooms/" + _roomId + "/confirm";

    JsonDocument payload;
    payload["meeting_id"] = meetingId;

    String payloadStr;
    serializeJson(payload, payloadStr);

    return makeRequest("POST", endpoint, payloadStr, doc);
}

bool APIClient::endMeeting(const String& meetingId) {
    JsonDocument doc;
    String endpoint = "/api/v1/rooms/" + _roomId + "/end-meeting";
    JsonDocument payload;
    payload["meeting_id"] = meetingId;

    String payloadStr;
    serializeJson(payload, payloadStr);

    return makeRequest("POST", endpoint, payloadStr, doc);
}
