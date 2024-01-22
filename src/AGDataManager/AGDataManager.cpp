#include "AGDataManager.h"

AGDataManager::AGDataManager(AGMQTTClient& mqttClientRef, AGWebClient& webClientRef) : mqttClient(mqttClientRef), webClient(webClientRef) {
    storedPackages.clear();
}

void AGDataManager::getPackage(AGPacket package, AGModule module) {
    std::pair<AGPacket, AGModule> pair(package, module);
    storedPackages.push_back(pair);
}

void AGDataManager::sendStoredPackage() {
    if(storedPackages.size() == 0) {
        return;
    }

    AGPacket package = storedPackages.front().first;
    AGModule module = storedPackages.front().second;

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, package.data);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    String timestamp = doc["timestamp"];
    float temperature = doc["temperature"];
    float airHumidity = doc["airHumidity"];
    float dirtHumidity = doc["dirtHumidity"];
    float voltage = doc["voltage"];
    float batteryPercentage = doc["batteryPercentage"];

    Serial.println("Received package:");
    Serial.print("Timestamp: "); Serial.println(timestamp);
    Serial.print("Temperature: "); Serial.println(temperature);
    Serial.print("Air Humidity: "); Serial.println(airHumidity);
    Serial.print("Dirt Humidity: "); Serial.println(dirtHumidity);
    Serial.print("Voltage: "); Serial.println(voltage);
    Serial.print("Battery Percentage: "); Serial.println(batteryPercentage);

    storedPackages.erase(storedPackages.begin());

    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(airHumidity) + ", \"dirt\": " + String(dirtHumidity * 100) + ", \"battery\": " + String(batteryPercentage) + "}";
    
    AGWebRequest request("/module/" + module.id + "/data", "POST", data, timestamp);
    webClient.storeRequest(request);
}