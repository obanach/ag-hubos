#ifndef AG_DATA_MANAGER_H
#define AG_DATA_MANAGER_H

#include <ArduinoJson.h>
#include "AGModule/AGModule.h"
#include "AGUtil/AGUtil.h"
#include "AGPacket/AGPacket.h"
#include "AGWebClient/AGWebClient.h"
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGWebRequest/AGWebRequest.h"

class AGDataManager {
public:
    AGDataManager(AGMQTTClient& mqttClientRef, AGWebClient& webClientRef);
    void getPackage(AGPacket package, AGModule module);
    void sendStoredPackage();
private:
    AGMQTTClient& mqttClient;
    AGWebClient& webClient;
    std::vector<std::pair<AGPacket, AGModule>> storedPackages;
};

#endif // AG_DATA_MANAGER_H