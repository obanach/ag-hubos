#ifndef AG_MQTT_CLIENT_H
#define AG_MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <vector>
#include "AGServerPacket/AGServerPacket.h"
#include "AGModule/AGModule.h"

class AGModuleManager;

class AGMQTTClient {
public:
    AGMQTTClient();
    void begin();
    void begin(const String& server, int port, const String& username, const String& password);
    void setCredentials(const String& id, const String& name, const String& username, const String& password);
    void loop();
    void publish(const String& topic, const String& payload);
    void publish(const AGServerPacket& packet);
    void subscribe(const String& topic);
    void unsubscribe(const String& topic);
    bool isConnected();
    void resetConnection();
    void tempDisconnect();
    void tempReconnect();
    void storePacket(const AGServerPacket& packet);
    void storePacket(const AGPacket& packet, const String& topic);
    void publishStoredPackets();
    void publishDiscoveredModules(std::vector<AGModule> modules);
    static void receiveMessage(char *topic, uint8_t *payload, unsigned int length);

    bool shouldReconnect;
    bool credentialsSet;
    String topic;
    void saveCredentials();
    bool loadCredentials();
    bool connecting;
    void setModuleManager(AGModuleManager* moduleManager);

    private:
    static AGMQTTClient* instance;
    String id;
    String name;
    String mqttBroker = "mqtt.autogrow.pl";
    String connectionTopic;
    String mqttUsername;
    String mqttPassword;
    int mqttPort = 1883;
    String clientId;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    std::vector<AGServerPacket> storedPackets;
    AGModuleManager* moduleManager;
};

#endif // AG_MQTT_CLIENT_H