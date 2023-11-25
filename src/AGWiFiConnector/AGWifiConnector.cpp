#include "AGWiFiConnector.h"

AGWiFiConnector::AGWiFiConnector() 
{
    shouldAttemptConnection = false;
    connectionAttemptFinished = false; 
    connectionSuccessful = false;
    startAttemptTime = 0;
}

void AGWiFiConnector::attemptConnection(const String& ssid, const String& password) {
    if (!shouldAttemptConnection) {
        WiFi.disconnect();
        WiFi.begin(ssid.c_str(), password.c_str());
        startAttemptTime = millis();
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
        } else if (millis() - startAttemptTime >= 10000) { // Connection timeout
            WiFi.disconnect();
            shouldAttemptConnection = false;
            connectionAttemptFinished = true;
        }
    }
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

String AGWiFiConnector::getConnectionHTML() {
    return "<html><body>"
            "<h1>Connecting...</h1>"
            "<script>"
            "setInterval(function() {"
            "fetch('/status').then(response => response.json()).then(data => {"
            "if (data.connected) {"
            "window.location.href = '/success';"
            "} else if (data.failed) {"
            "window.location.href = '/failure';"
            "}"
            "});"
            "}, 5000);"
            "</script>"
            "</body></html>";
}
