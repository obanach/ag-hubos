#ifndef AG_WEB_CLIENT_H
#define AG_WEB_CLIENT_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGModule/AGModule.h"
#include "AGPacket/AGPacket.h"
#include "AGWebRequest/AGWebRequest.h"

class AGModuleManager;

class AGWebClient {
public:
    AGWebClient(AGMQTTClient& mqttClientRef);
    void loop();
    bool sendVerificationCode(const String& verificationCode);
    bool deserializeResponse(const String& response);
    bool pingBackend();
    bool deleteHub();
    bool deleteModule(const AGModule module);
    bool pingModule(const AGModule module);
    String registerModule(const AGModule module);
    void saveInfo();
    void loadInfo();
    void storeRequest(AGWebRequest request);
    bool credentialsSet = false;
    void setModuleManager(AGModuleManager* moduleManager);
    bool resetFlag = false;

private:
    String id;
    String name;
    String accessToken;
    String mqttUsername;
    String mqttPassword;
    std::vector<AGWebRequest> storedRequests;
    HTTPClient http;
    String baseUrl = "https://api.autogrow.pl/device/";
    AGMQTTClient& mqttClient;
    AGModuleManager* moduleManager;
};

#endif // AG_WEB_CLIENT_H
