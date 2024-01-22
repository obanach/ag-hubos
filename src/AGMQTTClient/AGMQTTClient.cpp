#include "AGMQTTClient.h"
#include "AGModuleManager/AGModuleManager.h"

AGMQTTClient* AGMQTTClient::instance = nullptr;

AGMQTTClient::AGMQTTClient() : mqttClient(wifiClient), moduleManager(nullptr) {
    instance = this;
    shouldReconnect = true;
    credentialsSet = false;
}

void AGMQTTClient::begin() {
    if(!shouldReconnect || !credentialsSet || connecting) return;
    connecting = true;
    Serial.println("AGMQTTClient trying to connect to MQTT broker...");
    mqttClient.setServer(mqttBroker.c_str(), mqttPort);
    mqttClient.setCallback(receiveMessage);
    mqttClient.setBufferSize(2048);
    clientId = "hub-";
    clientId += String(WiFi.macAddress());
    if (mqttClient.connect(clientId.c_str(), mqttUsername.c_str(), mqttPassword.c_str())) {
        Serial.println("AutoGrow EMQX MQTT broker connected");
        subscribe(topic + "/#");
    } else {
        Serial.print("Failed with state ");
        Serial.println(mqttClient.state());
        if(mqttClient.state() == 5) {
            credentialsSet = false;
        }
    }
    connecting = false;
    shouldReconnect = true;
}

void AGMQTTClient::setCredentials(const String& id, const String& name, const String& username, const String& password) {
    this->id = id;
    this->name = name;
    this->mqttUsername = username;
    this->mqttPassword = password;
    this->topic = "hub/" + id;
    this->connectionTopic = topic + "/connection";
    saveCredentials();
    Serial.println("MQTT credentials set: \n" + id + "\n" + name + "\n" + username + "\n" + password);
    credentialsSet = true;
}

void AGMQTTClient::loop() {
    if(connecting && mqttClient.connected()) connecting = false;
    if(mqttClient.connected()) {
        publishStoredPackets();
    }
    mqttClient.loop();
}

void AGMQTTClient::receiveMessage(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    String topicString = String(topic);
    std::vector<String> topicParts;
    int slashIndex = 0;
    int lastSlashIndex = -1;

    while (slashIndex != -1) {
        slashIndex = topicString.indexOf("/", slashIndex + 1);

        if (slashIndex != -1)
            topicParts.push_back(topicString.substring(lastSlashIndex + 1, slashIndex));
        else
            topicParts.push_back(topicString.substring(lastSlashIndex + 1));

        lastSlashIndex = slashIndex;
    }

    // hub/id/topic1/topic2/...
    String firstTopic = topicParts.size() > 2 ? topicParts[2] : "";
    String secondTopic = topicParts.size() > 3 ? topicParts[3] : "";
    Serial.println("First topic: \"" + firstTopic + "\", second topic: \"" + secondTopic + "\".");
    Serial.print("Message: ");
    String message = "";
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        message += (char)payload[i];
    }
    Serial.println();
    Serial.println("-----------------------");

    AGServerPacket packet(message);
    Serial.println("Packet type: " + packet.responseType);
    if(packet.responseType == "request") {
        if(firstTopic == "module") {
            if(secondTopic == "discover") {
                instance->moduleManager->forceDiscoverModules();
            } else 
            if(secondTopic == "pair") {
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, packet.contents);
                if(doc["macAddress"].isNull()) {
                    Serial.println("MAC address not found in packet.");
                    return;
                }
                String macAddress = doc["macAddress"].as<String>();
                
                instance->moduleManager->connectModule(macAddress);
            } else
            if(secondTopic == "delete") {
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, packet.contents);

                if(doc["macAddress"].isNull()) {
                    Serial.println("MAC address not found in packet.");
                    if(doc["id"].isNull()) {
                        Serial.println("ID not found in packet.");
                        return;
                    }
                    String id = doc["id"].as<String>();
                    AGModule module = instance->moduleManager->findModuleByID(id);
                    if(module.macAddress != nullptr) {
                        instance->moduleManager->disconnectModule(macToString(module.macAddress));
                    } else {
                        Serial.println("Module not found.");
                    }
                } else {
                    String macAddress = doc["macAddress"].as<String>();
                    instance->moduleManager->disconnectModule(macAddress);
                }
            }
        }
    }
}

void AGMQTTClient::publish(const String& topic, const String& payload) {
    if(!mqttClient.connected() || !WiFi.isConnected()) 
        storePacket(AGPacket(payload), topic);
    else
        if(mqttClient.publish(topic.c_str(), payload.c_str()))
            Serial.println("Packet sent.");
        else
            Serial.println("Packet not sent.");
}

void AGMQTTClient::publish(const AGServerPacket& packet) {
    if(!mqttClient.connected() || !WiFi.isConnected()) 
        storePacket(packet);
    else {
        if(mqttClient.publish(packet.topic.c_str(), packet.toString().c_str()))
            Serial.println("Packet sent.");
        else
            Serial.println("Packet not sent.");
    }
}

void AGMQTTClient::storePacket(const AGServerPacket& packet) {
    storedPackets.push_back(packet);
}

void AGMQTTClient::storePacket(const AGPacket& packet, const String& topic) {
    AGServerPacket serverPacket(packet);
    serverPacket.topic = topic;
    storedPackets.push_back(serverPacket);
}

void AGMQTTClient::publishDiscoveredModules(std::vector<AGModule> modules) {
    AGServerPacket packet;
    packet.contentsFromModuleVector(modules);
    packet.topic = topic + "/module/discover";
    publish(packet);
}

void AGMQTTClient::publishStoredPackets() {
    if(storedPackets.size() == 0) {
        return;
    }

    for(int i = 0; i < storedPackets.size(); i++) {
        Serial.println("Sending packet to MQTT.");
        mqttClient.publish(storedPackets[i].topic.c_str(), storedPackets[i].toString().c_str());
    }

    storedPackets.clear();
    Serial.println("All stored packets sent.");
}

void AGMQTTClient::subscribe(const String& topic) {
    mqttClient.subscribe(topic.c_str());
}

void AGMQTTClient::unsubscribe(const String& topic) {
    mqttClient.unsubscribe(topic.c_str());
}

bool AGMQTTClient::isConnected() {
    return mqttClient.connected();
}

void AGMQTTClient::resetConnection() {
    shouldReconnect = false;
    credentialsSet = false;
    mqttClient.disconnect();
    id = "";
    name = "";
    mqttUsername = "";
    mqttPassword = "";
    topic = "";
    connectionTopic = "";
    clientId = "";
    saveCredentials();
    Serial.println("MQTT credentials reset.");
}

void AGMQTTClient::tempDisconnect() {
    if(mqttClient.connected()) {
        shouldReconnect = false;
        mqttClient.disconnect();
    }
}

void AGMQTTClient::tempReconnect() {
    connecting = true;
    if(mqttClient.connect(clientId.c_str(), mqttUsername.c_str(), mqttPassword.c_str())) {
        Serial.println("AutoGrow EMQX MQTT broker reconnected");
        subscribe(topic + "/#");
    } else {
        Serial.print("Failed with state ");
        Serial.println(mqttClient.state());
    }
    connecting = false;
    shouldReconnect = true;
}

void AGMQTTClient::saveCredentials() {
    Preferences preferences;
    preferences.begin("mqtt", false);
    preferences.putString("id", id);
    preferences.putString("name", name);
    preferences.putString("username", mqttUsername);
    preferences.putString("password", mqttPassword);
    preferences.end();
}

bool AGMQTTClient::loadCredentials() {
    Preferences preferences;
    preferences.begin("mqtt", true);
    if (!preferences.isKey("id") || !preferences.isKey("name") || !preferences.isKey("username") || !preferences.isKey("password")) {
        Serial.println("MQTT credentials not found in memory.");
        shouldReconnect = false;
        preferences.end();
        return false;
    }
    credentialsSet = true;
    id = preferences.getString("id");
    name = preferences.getString("name");
    mqttUsername = preferences.getString("username");
    mqttPassword = preferences.getString("password");
    setCredentials(id, name, mqttUsername, mqttPassword);
    preferences.end();
    return true;
}

void AGMQTTClient::setModuleManager(AGModuleManager* moduleManager) {
    this->moduleManager = moduleManager;
}
