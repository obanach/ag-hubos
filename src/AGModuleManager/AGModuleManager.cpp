// AGModuleManager.cpp
#include "AGModuleManager.h"

AGModuleManager* AGModuleManager::instance = nullptr;

AGModuleManager::AGModuleManager(AGDataManager& dataManagerRef) : dataManager(dataManagerRef) {
    instance = this;
}

void AGModuleManager::initEspNow() {
    // Initialize ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    Serial.println("ESP-NOW initialized. My MAC address is: " + WiFi.macAddress());
    esp_err_t error = esp_now_register_recv_cb(OnDataRecv);

    if (error != ESP_OK) {
        Serial.println("Error registering ESP-NOW callback");
        return;
    } else {
        Serial.println("ESP-NOW callback registered");
    }
}

void AGModuleManager::discoverModules() {
    broadcastMessage("DISCOVER");
}

bool AGModuleManager::broadcastMessage(const String& message) {
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast address
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    if(!esp_now_is_peer_exist(broadcastAddress)) {
        esp_now_add_peer(&peerInfo);
    }

    esp_err_t error = esp_now_send(broadcastAddress, (uint8_t *)message.c_str(), message.length() + 1);

    if(error != ESP_OK) {
        Serial.println("Error sending ESP-NOW message: " + String(error));
    } else {
        Serial.println("ESP-NOW message " + message + " sent successfully");
    }

    return error == ESP_OK;
}

bool AGModuleManager::sendMessage(const String& message, const String& macAddress) {
    uint8_t newMac[6];
    stringToMac(macAddress, newMac);
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, newMac, 6);
    if(!esp_now_is_peer_exist(newMac)) {
        esp_now_add_peer(&peerInfo);
    }

    esp_err_t error = esp_now_send(newMac, (uint8_t *)message.c_str(), message.length() + 1);

    if(error != ESP_OK) {
        Serial.println("Error sending ESP-NOW message: " + String(error));
    } else {
        Serial.println("ESP-NOW message " + message + " sent successfully");
    }
    
    return error == ESP_OK;
}

void AGModuleManager::connectModule(const String& macAddress) {
    // Check if the module with the given MAC address is already connected
    uint8_t newMac[6];
    stringToMac(macAddress, newMac);
    auto it = std::find_if(
        connectedModules.begin(), 
        connectedModules.end(), 
        [&newMac](const AGModule& m) {
            return std::equal(std::begin(m.macAddress), std::end(m.macAddress), std::begin(newMac));
        }
    );

    // If the module is not already connected, create a new AGModule object and add it to the list
    if (it == connectedModules.end()) {
        if(sendMessage("PAIR", macAddress))
            Serial.println("Module pairing request sent: " + macAddress);
    } else {
        Serial.println("Module already connected: " + macAddress);
    }
}

void AGModuleManager::disconnectModule(const String& macAddress) {
    // Remove the module with the given MAC address from the list of connected modules
    uint8_t newMac[6];
    stringToMac(macAddress, newMac);

    connectedModules.erase(
        std::remove_if(
            connectedModules.begin(), 
            connectedModules.end(), 
            [&newMac](const AGModule& m) {
                return std::equal(std::begin(m.macAddress), std::end(m.macAddress), std::begin(newMac));
            }
        ), 
        connectedModules.end()
    );
}

std::vector<AGModule> AGModuleManager::getDiscoveredModuleList() {
    return discoveredModules;
}

void AGModuleManager::OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
    String message = String((char*)incomingData);
    Serial.println("Data received from: " + macToString(mac_addr) + " - " + message);
    if (!instance) return;
    if (message == "ESP8266_HERE") {
        bool found = false;
        for (auto& module : instance->discoveredModules) {
            if (module.isMacAddressEqual(mac_addr)) {
                found = true;
                Serial.println("ESP8266 module already discovered: " + macToString(mac_addr));
                break;
            }
        }

        if (!found) {
            AGModule agModule(message, "ESP8266", mac_addr);
            instance->discoveredModules.push_back(agModule);
            Serial.println("ESP8266 module discovered and added to the list: " + macToString(mac_addr));
        }
    } else if(message == "PAIR_OK") {
        bool found = false;
        for (auto& module : instance->connectedModules) {
            if (module.isMacAddressEqual(mac_addr)) {
                found = true;
                Serial.println("Module already connected: " + macToString(mac_addr));
                break;
            }
        }

        if (!found) {
            AGModule agModule(message, "ESP8266", mac_addr);
            instance->connectedModules.push_back(agModule);
            Serial.println("Module connected and added to the list: " + macToString(mac_addr));
        }
    } else if(message == "PAIR_KO") {
        Serial.println("Module pairing failed: " + macToString(mac_addr));
    } else if(message.substring(0, 4) == "DATA") {
        Serial.println("Data received from module: " + macToString(mac_addr));
        String data = message.substring(5);
        instance->dataManager.printPackage(data);
    } else {
        Serial.println("Unknown message received: " + message);
    }
}

void AGModuleManager::sendPackageRequests() {
    for (auto& module : connectedModules) {
        sendMessage("PACKAGE_SEND", macToString(module.macAddress));
    }
}