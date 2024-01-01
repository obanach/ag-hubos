#include "AGModule.h"

std::vector<String> splitString(const String& str, char delimiter) {
    std::vector<String> elements;
    String element;
    for (unsigned int i = 0; i < str.length(); i++) {
        char ch = str.charAt(i);
        if (ch == delimiter) {
            if (element.length() > 0) {
                elements.push_back(element);
                element = "";
            }
        } else {
            element += ch;
        }
    }
    if (element.length() > 0) {
        elements.push_back(element);
    }
    return elements;
}

AGModule::AGModule(String name, String type, const uint8_t *macAddress) : name(name), type(type) {
    memcpy(this->macAddress, macAddress, 6);
}

AGModule::AGModule(const String& serializedModule) {
    std::vector<String> parts = splitString(serializedModule, ';');
    if (parts.size() >= 3) {
        name = parts[0];
        type = parts[1];

        // Split the MAC address
        std::vector<String> macParts = splitString(parts[2], ':');
        if (macParts.size() == 6) {
            for (int i = 0; i < 6; ++i) {
                macAddress[i] = (uint8_t) strtol(macParts[i].c_str(), nullptr, 16);
            }
        }
    }
}

bool AGModule::isMacAddressEqual(const uint8_t* otherMac) const {
    if (otherMac == nullptr) return false;
    for (int i = 0; i < 6; ++i) {
        if (this->macAddress[i] != otherMac[i]) return false;
    }
    return true;
}

String AGModule::toString() {
    String moduleStr = name + ";" + type + ";";
    for (int i = 0; i < 6; ++i) {
        moduleStr += String(macAddress[i], HEX);
        if (i < 5) moduleStr += ":";
    }
    return moduleStr;
}
