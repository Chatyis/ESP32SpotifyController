#include "WifiManager.h"
#include <WiFi.h>

WifiManager::WifiManager() {
}

WifiManager::~WifiManager() {
}

void WifiManager::connectToWifi(const char* ssid, const char* password) {
    _ssid = (char*)ssid;
    _password = (char*)password;
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void WifiManager::checkIfDisconnected() {
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi Disconnected");
        WiFi.begin(_ssid, _password);
        Serial.println("Reconnecting");

        while(WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }
}