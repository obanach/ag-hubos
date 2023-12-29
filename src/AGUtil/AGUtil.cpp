#include "AGUtil.h"

String macToString(const uint8_t* mac) {
    if (mac == nullptr) return String("00:00:00:00:00:00");
    char macStr[18] = {0};
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

bool stringToMac(const String& macStr, uint8_t* mac) {
    if (macStr.length() != 17) // "XX:XX:XX:XX:XX:XX" = 17 chars
        return false;

    unsigned int macBytes[6];
    if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &macBytes[0], &macBytes[1], &macBytes[2], &macBytes[3], &macBytes[4], &macBytes[5]) == 6) {
        for (int i = 0; i < 6; ++i)
            mac[i] = static_cast<uint8_t>(macBytes[i]);
        return true;
    }

    return false;
}
