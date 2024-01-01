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
    discoveredModules.clear();
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

    if(sendMessage("UNPAIR", macAddress))
        Serial.println("Module disconnection request sent: " + macAddress);
}

bool AGModuleManager::isModuleConnected(const String& macAddress) {
    for (const auto& module : connectedModules)
        if (macToString(module.macAddress) == macAddress)
            return true;
            
    return false;
}

std::vector<AGModule> AGModuleManager::getDiscoveredModuleList() {
    return discoveredModules;
}

std::vector<AGModule> AGModuleManager::getConnectedModuleList() {
    return connectedModules;
}

void AGModuleManager::saveModulesToMemory() {
    savedModules.begin("modules", false);
    savedModules.clear();
    Serial.println("Saving modules to memory.");

    for (size_t i = 0; i < connectedModules.size(); i++) {
        String key = "module" + String(i);
        String value = connectedModules[i].toString();
        savedModules.putString(key.c_str(), value);
    }

    Serial.println("Modules saved to memory: " + String(connectedModules.size()));

    savedModules.end();
}

void AGModuleManager::loadModulesFromMemory() {
    savedModules.begin("modules", true);
    Serial.println("Reading modules from memory.");

    connectedModules.clear();

    for (size_t i = 0; ; ++i) {
        String key = "module" + String(i);
        
        if (!savedModules.isKey(key.c_str()))
            break;

        String serializedModule = savedModules.getString(key.c_str());
        AGModule module(serializedModule);
        connectedModules.push_back(module);
    }

    Serial.println("Modules loaded from memory: " + String(connectedModules.size()));

    savedModules.end();
}

void AGModuleManager::OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
    String message = String((char*)incomingData);
    Serial.println("Data received from: " + macToString(mac_addr) + " - " + message);
    if (!instance) return;
    if (message.substring(0, 6) == "MODULE") {
        String moduleInfo = message.substring(6);
        AGModule agModule(moduleInfo);
        bool found = false;
        for (auto& module : instance->discoveredModules) {
            if (module.isMacAddressEqual(mac_addr)) {
                found = true;
                Serial.println("ESP8266 module already discovered: " + macToString(mac_addr));
                break;
            }
        }

        if (!found) {
            instance->discoveredModules.push_back(agModule);
            Serial.println("ESP8266 module discovered and added to the list: " + macToString(mac_addr));
        }
    } else if(message.substring(0, 7) == "PAIR_OK") {
        String moduleInfo = message.substring(7);
        AGModule agModule(moduleInfo);
        bool found = false;
        for (auto& module : instance->connectedModules) {
            if (module.isMacAddressEqual(mac_addr)) {
                found = true;
                Serial.println("Module already connected: " + macToString(mac_addr));
                break;
            }
        }

        if (!found) {
            instance->connectedModules.push_back(agModule);
            Serial.println("Module connected and added to the list: " + macToString(mac_addr));
            instance->saveModulesToMemory();
        }

        instance->discoverModules();
    } else if(message == "PAIR_KO") {
        Serial.println("Module pairing failed: " + macToString(mac_addr));
    } else if(message == "UNPAIR_OK") {
        uint8_t newMac[6];
        memcpy(newMac, mac_addr, 6);
        instance->connectedModules.erase(
        std::remove_if(
            instance->connectedModules.begin(), 
            instance->connectedModules.end(), 
            [&newMac](const AGModule& m) {
                return std::equal(std::begin(m.macAddress), std::end(m.macAddress), std::begin(newMac));
            }), 
            instance->connectedModules.end()
        );
        instance->discoverModules();
        Serial.println("Module disconnected: " + macToString(mac_addr));
    } else if(message.substring(0, 4) == "DATA") {
        Serial.println("Data received from module: " + macToString(mac_addr));
        String data = message.substring(4);
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