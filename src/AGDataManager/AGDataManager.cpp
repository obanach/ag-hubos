#include "AGDataManager.h"

AGDataManager::AGDataManager() {
    // Constructor
}

void AGDataManager::printPackage(String package) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, package);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    long timestamp = doc["timestamp"];
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
}