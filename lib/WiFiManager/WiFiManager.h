//
// Created by Oliwer Banach on 01/11/2023.
//

#ifndef WifiManager_h
#define WifiManager_h

#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
public:
    WifiManager(const char* ssid, const char* password);
    void connect();
    void disconnect();

private:
    const char* _ssid;
    const char* _password;
};

#endif
