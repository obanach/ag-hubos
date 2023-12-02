#include "AGWiFiConnector.h"

AGWiFiConnector::AGWiFiConnector() 
{
    shouldAttemptConnection = false;
    connectionAttemptFinished = false; 
    connectionSuccessful = false;
    startAttemptTime = 0;
}

bool AGWiFiConnector::checkCredentials() {
    wifiCredentials.begin("wifi", false);
    String ssid = wifiCredentials.getString("ssid", "");
    String password = wifiCredentials.getString("password", "");
    wifiCredentials.end();
    return ssid.length() > 0 && password.length() > 0;
}

void AGWiFiConnector::saveCredentials(const String& ssid, const String& password) {
    wifiCredentials.begin("wifi", false);
    wifiCredentials.putString("ssid", ssid);
    wifiCredentials.putString("password", password);
    wifiCredentials.end();
}

void AGWiFiConnector::clearCredentials() {
    wifiCredentials.begin("wifi", false);
    wifiCredentials.clear();
    wifiCredentials.end();
}

String AGWiFiConnector::getSavedSSID() {
    wifiCredentials.begin("wifi", false);
    String ssid = wifiCredentials.getString("ssid", "");
    wifiCredentials.end();
    return ssid;
}

String AGWiFiConnector::getSavedPassword() {
    wifiCredentials.begin("wifi", false);
    String password = wifiCredentials.getString("password", "");
    wifiCredentials.end();
    return password;
}

void AGWiFiConnector::attemptConnection(const String& ssid, const String& password) {
    if (!shouldAttemptConnection) {
        WiFi.disconnect();
        WiFi.begin(ssid.c_str(), password.c_str());
        startAttemptTime = millis();
        ssidToSave = ssid;
        passwordToSave = password;
        shouldAttemptConnection = true;
        connectionAttemptFinished = false;
        connectionSuccessful = false;
    }
}

void AGWiFiConnector::updateConnection() {
    if (shouldAttemptConnection && !connectionAttemptFinished) {
        if (WiFi.status() == WL_CONNECTED) {
            connectionSuccessful = true;
            shouldAttemptConnection = false;
            connectionAttemptFinished = true;
            Serial.println("Connected to WiFi successfully.");
            saveCredentials(ssidToSave, passwordToSave);
        } else if (millis() - startAttemptTime >= 10000) { // Connection timeout
            WiFi.disconnect();
            shouldAttemptConnection = false;
            connectionAttemptFinished = true;
            Serial.println("Connection to WiFi failed.");
            clearCredentials();
        }
    }
}

void AGWiFiConnector::resetConnection() {
    WiFi.disconnect();
    shouldAttemptConnection = false;
    connectionAttemptFinished = false;
    connectionSuccessful = false;
}

bool AGWiFiConnector::isConnected() {
    return connectionSuccessful;
}

bool AGWiFiConnector::isConnectionAttemptFinished() {
    return connectionAttemptFinished;
}

String AGWiFiConnector::getConnectionStatusJSON() {
    if (connectionAttemptFinished) {
        if (connectionSuccessful) {
            return "{\"connected\": true}";
        } else {
            return "{\"failed\": true}";
        }
    }
    return "{\"connecting\": true}";
}