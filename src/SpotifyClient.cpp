#include "SpotifyClient.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DisplayManager.h>

SpotifyClient::SpotifyClient(String refreshToken, String authBasicToken) {
    _refreshToken = refreshToken;
    _authBasicToken = authBasicToken;
}

SpotifyClient::~SpotifyClient() {
}

void SpotifyClient::togglePause() {
    if(isPaused) {
        isPaused = false;
        play();
    } else {
        isPaused = true;
        pause();
    }
}

void SpotifyClient::skipToNextSong(DisplayManager& displayManager) {
    displayManager.displayChangingSong();
    _postRequest("/me/player/next");
    delay(100);
    getCurrentSong(displayManager);
}

void SpotifyClient::skipToPreviousSong(DisplayManager& displayManager) {
    displayManager.displayChangingSong();
    _postRequest("/me/player/previous");
    delay(100);
    getCurrentSong(displayManager);
}

void SpotifyClient::play() {
    _putRequest("/me/player/play");
}

void SpotifyClient::pause() {
    _putRequest("/me/player/pause");
}

void SpotifyClient::getCurrentSong(DisplayManager& displayManager) {
    JsonDocument doc;
    String payload = _getRequest("/me/player/currently-playing");

    if(payload == "") {
        return;
    }

    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        _http.end();
        return;
    }
    //TODO display all artists - just concat with ", "
    passedTrackTime = doc["progress_ms"].as<int>();
    totalTrackTime = doc["item"]["duration_ms"].as<int>();
    isPaused = !doc["is_playing"].as<bool>();
    displayManager.updateSong(doc["item"]["name"].as<String>(), doc["item"]["artists"][0]["name"].as<String>());
}

// void SpotifyClient::getPlaybackState() {
//
//     _getRequest("/me/player");
// }

void SpotifyClient::refreshAccessToken() {
    String endpoint = "/token";
    String params = "?grant_type=refresh_token&refresh_token=" + _refreshToken;
    // String headers = "Authorization: Basic " + _authBasicToken  + "\nContent-Type: application/x-www-form-urlencoded";
    _http.begin((_authServerName + endpoint + params).c_str());

    _http.addHeader("Authorization", _authBasicToken);
    _http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    _http.addHeader("Content-Length", "0");

    int httpResponseCode = _http.POST(nullptr,0);

    if(_handleResponseCode(httpResponseCode)) {
        JsonDocument doc;
        String payload = _http.getString();

        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            _http.end();
            return;
        }
        _accessToken = doc["access_token"].as<String>();
    }

    _http.end();
}

void SpotifyClient::_getAvailableDevices() {
    String endpoint = "/me/player/previous";

    _http.begin((_serverName + endpoint).c_str());

    //_http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

    int httpResponseCode = _http.GET();

    _handleResponseCode(httpResponseCode);

    _http.end();
}

bool SpotifyClient::_handleResponseCode(int httpResponseCode) {
    switch(httpResponseCode) {
        case 200: {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            // String payload = _http.getString();
            // Serial.println(payload);
            return true;
        }
        case 401:
            // TODO refresh token, but prevent infinite loop
            // refreshAccessToken();
            Serial.println("HTTP Response 403: Forbidden");
        break;
        case 403:
            Serial.println("HTTP Response 403: Forbidden");
        break;
        case 404:
            Serial.println("HTTP Response 404: Not Found");
        break;
        default:
            Serial.println("HTTP Response code: " + String(httpResponseCode));
        String payload = _http.getString();
        Serial.println(payload);
        break;
    }
    return false;
}

void SpotifyClient::_addAuthHeaders() {
    _http.addHeader("Authorization", "Bearer " + _accessToken);
    _http.addHeader("Content-Length", "0");
}

String SpotifyClient::_getRequest(const String& endpoint) {
    String payloadResponse = "";
    _http.begin((_serverName + endpoint).c_str());

    _addAuthHeaders();

    int httpResponseCode = _http.GET();

    if(_handleResponseCode(httpResponseCode)) {
        payloadResponse = _http.getString();
    }

    _http.end();

    return payloadResponse;
}

void SpotifyClient::_postRequest(const String& endpoint) {
    _http.begin((_serverName + endpoint).c_str());

    Serial.printf(endpoint.c_str());
    _addAuthHeaders();

    int httpResponseCode = _http.POST("");

    _handleResponseCode(httpResponseCode);

    _http.end();
}

void SpotifyClient::_putRequest(const String& endpoint) {
    _http.begin((_serverName + endpoint).c_str());

    _addAuthHeaders();

    int httpResponseCode = _http.PUT("");

    _handleResponseCode(httpResponseCode);

    _http.end();
}