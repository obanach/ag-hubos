#ifndef AG_WEB_CLIENT_H
#define AG_WEB_CLIENT_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGModule/AGModule.h"
#include "AGPacket/AGPacket.h"

class AGWebClient {
public:
    AGWebClient(AGMQTTClient& mqttClientRef);
    bool sendVerificationCode(const String& verificationCode);
    bool deserializeResponse(const String& response);
    bool pingBackend();
    bool deleteHub();
    bool deleteModule(const AGModule module);
    bool pingModule(const AGModule module);
    bool addModule(const AGModule module);
    bool sendData(const AGPacket data);
    void saveInfo();
    void loadInfo();
    bool credentialsSet = false;

private:
    String id;
    String name;
    String accessToken;
    String mqttUsername;
    String mqttPassword;
    HTTPClient http;
    String baseUrl = "https://api.autogrow.pl/device/hub/pair";
    AGMQTTClient& mqttClient;
};

#endif // AG_WEB_CLIENT_H
