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
    mqttClient.setCallback(callback);
    clientId = "hub-";
    clientId += String(WiFi.macAddress());
    if (mqttClient.connect(clientId.c_str(), mqttUsername.c_str(), mqttPassword.c_str())) {
        Serial.println("AutoGrow EMQX MQTT broker connected");
        subscribe(topic + "/#");
        publish(connectionTopic, "Hub online.");
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
    mqttClient.loop();
}

void AGMQTTClient::callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.print(topic);
    Serial.print(" / ");
    String topicString = String(topic);
    int lastSlashIndex = topicString.lastIndexOf("/");
    String lastTopic = topicString.substring(lastSlashIndex + 1);
    Serial.println(lastTopic);
    Serial.print("Message: ");
    String message = "";
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        message += (char)payload[i];
    }
    Serial.println();
    Serial.println("-----------------------");
}

void AGMQTTClient::publish(const String& topic, const String& payload) {
    mqttClient.publish(topic.c_str(), payload.c_str());
}

void AGMQTTClient::publish(const AGServerPacket& packet) {
    mqttClient.publish(packet.topic.c_str(), packet.toString().c_str());
}

void AGMQTTClient::storePacket(const AGServerPacket& packet) {
    storedPackets.push_back(packet);
}

void AGMQTTClient::storePacket(const AGPacket& packet, const String& topic) {
    AGServerPacket serverPacket(packet);
    serverPacket.topic = topic;
    storedPackets.push_back(serverPacket);
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

String AGMQTTClient::getConnectionStatusJSON() {
    String status = "{\"connected\":";
    status += isConnected() ? "true" : "false";
    status += "}";
    return status;
}

void AGMQTTClient::resetConnection() {
    mqttClient.disconnect();
    shouldReconnect = false;
    credentialsSet = false;
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
    shouldReconnect = false;
    if(mqttClient.connected()) mqttClient.disconnect();
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
