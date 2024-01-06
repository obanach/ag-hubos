// AGModuleManager.h
#ifndef AG_MODULE_MANAGER_H
#define AG_MODULE_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <esp_now.h>
#include <Preferences.h>
#include <WiFi.h>
#include "AGModule/AGModule.h"
#include "AGUtil/AGUtil.h"
#include "AGDataManager/AGDataManager.h"
#include "AGPacket/AGPacket.h"

class AGModuleManager {
public:
    AGModuleManager(AGDataManager& dataManagerRef);
    void initEspNow();
    void discoverModules();
    bool broadcastMessage(const String& message);
    bool sendMessage(const String& message, const String& macAddress);
    void connectModule(const String& macAddress);
    void disconnectModule(const String& macAddress);
    bool isModuleConnected(const String& macAddress);
    void sendPackageRequests();
    std::vector<AGModule> getDiscoveredModuleList();
    std::vector<AGModule> getConnectedModuleList();
    void saveModulesToMemory();
    void loadModulesFromMemory();
    static AGModuleManager* instance;

private:
    Preferences savedModules;
    AGDataManager& dataManager;
    std::vector<AGModule> discoveredModules;
    std::vector<AGModule> connectedModules;
    static void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);
};

#endif // AG_MODULE_MANAGER_H
