//
// Created by Oliwer Banach on 01/11/2023.
//

#include "WifiManager.h"

WifiManager::WifiManager(const char* ssid, const char* password)
        : _ssid(ssid), _password(password) {}

void WifiManager::connect() {
    Serial.println("Connecting to WiFi...");

    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void WifiManager::disconnect() {
    WiFi.disconnect();
    Serial.println("WiFi disconnected.");
}