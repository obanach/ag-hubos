// AGModule.h
#ifndef AG_MODULE_H
#define AG_MODULE_H

#include <Arduino.h>
#include <vector>
#include <esp_now.h>
#include <WiFi.h>
#include "AGUtil/AGUtil.h"

class AGModule {
public:
    AGModule(String name, String type, const uint8_t *macAddress);
    AGModule(const String& serializedModule);
    String name;
    String type;
    uint8_t macAddress[6];

    String toString();
    bool isMacAddressEqual(const uint8_t *otherMac) const;
};

#endif // AG_MODULE_H