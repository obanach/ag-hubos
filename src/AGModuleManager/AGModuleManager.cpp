// AGModuleManager.cpp
#include "AGModuleManager.h"
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGConnectionSwitcher/AGConnectionSwitcher.h"

AGModuleManager* AGModuleManager::instance = nullptr;

AGModuleManager::AGModuleManager(AGDataManager& dataManagerRef) : dataManager(dataManagerRef), mqttClient(nullptr), connectionSwitcher(nullptr) {
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
        if(sendMessage(AGPacket("PAIR"), macAddress))
            Serial.println("Module pairing request sent: " + macAddress);
    } else {
        Serial.println("Module already connected: " + macAddress);
    }
}

void AGModuleManager::disconnectModule(const String& macAddress, bool force /* = false */) {
    // Remove the module with the given MAC address from the list of connected modules
    uint8_t newMac[6];
    stringToMac(macAddress, newMac);

    connectionSwitcher->forceEspNow(500);
    if(force) {
        connectedModules.erase(
        std::remove_if(
            connectedModules.begin(), 
            connectedModules.end(), 
            [&newMac](const AGModule& m) {
                return std::equal(std::begin(m.macAddress), std::end(m.macAddress), std::begin(newMac));
            }), 
            connectedModules.end()
        );
    }

    if(sendMessage(AGPacket("UNPAIR"), macAddress))
        Serial.println("Module disconnection request sent: " + macAddress);
}

void AGModuleManager::disconnectAllModules() {
    for (auto& module : connectedModules) {
        disconnectModule(macToString(module.macAddress), true);
    }
}

void AGModuleManager::clearModules() {
    disconnectAllModules();
    connectedModules.clear();
    discoveredModules.clear();
    saveModulesToMemory();
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
    AGPacket packet((char*)incomingData);
    Serial.println("Data received from: " + macToString(mac_addr) + " - " + packet.header);
    if (!instance) return;
    if (packet.header == "MODULE") {
        AGModule agModule(packet.data);
        Serial.println("Module info: " + packet.data + " - " + agModule.toString());
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
    } else if(packet.header == "PAIR_OK") {
        String moduleInfo = packet.data;
        Serial.println("Module info: " + moduleInfo);
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
            AGServerPacket serverPacket;
            serverPacket.contentsFromModuleVector(instance->connectedModules);
            Serial.println(serverPacket);
            instance->saveModulesToMemory();
        }

        instance->discoverModules();
    } else if(packet.header == "PAIR_KO") {
        Serial.println("Module pairing failed: " + macToString(mac_addr));
    } else if(packet.header == "UNPAIR_OK") {
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
    } else if(packet.header == "DATA") {
        instance->dataManager.getPackage(packet);
    } else if(packet.header == "NO_PACKAGES") {

    } else {
        Serial.println("Unknown message received: " + packet.toString());
    }
}

void AGModuleManager::sendPackageRequests() {
    for (auto& module : connectedModules) {
        sendMessage(AGPacket("PACKAGE_SEND"), macToString(module.macAddress));
    }
}

void AGModuleManager::setMQTTClient(AGMQTTClient* mqttClient) {
    this->mqttClient = mqttClient;
}

void AGModuleManager::setConnectionSwitcher(AGConnectionSwitcher* connectionSwitcher) {
    this->connectionSwitcher = connectionSwitcher;
}
