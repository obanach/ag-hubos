#include "AGModule.h"

AGModule::AGModule(String name, String type, const uint8_t *macAddress) : name(name), type(type) {
    memcpy(this->macAddress, macAddress, 6);
}

bool AGModule::isMacAddressEqual(const uint8_t* otherMac) const {
    if (otherMac == nullptr) return false;
    for (int i = 0; i < 6; ++i) {
        if (this->macAddress[i] != otherMac[i]) return false;
    }
    return true;
}