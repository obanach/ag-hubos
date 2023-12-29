// AGModule.h
#ifndef AG_MODULE_H
#define AG_MODULE_H

#include <Arduino.h>
#include <vector>
#include <esp_now.h>
#include <WiFi.h>

class AGModule {
public:
    AGModule(String name, String type, const uint8_t *macAddress);
    String name;
    String type;
    uint8_t macAddress[6];

    bool isMacAddressEqual(const uint8_t *otherMac) const;
};

#endif // AG_MODULE_H