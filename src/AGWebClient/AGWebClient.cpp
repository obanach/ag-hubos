#include "AGWebClient.h"

AGWebClient::AGWebClient(AGMQTTClient& mqttClientRef) : mqttClient(mqttClientRef) {}

void AGWebClient::saveInfo() {
    if(!credentialsSet) return;
    Preferences preferences;
    preferences.begin("web", false);
    preferences.putString("id", id);
    preferences.putString("name", name);
    preferences.putString("token", accessToken);
    preferences.end();
}

void AGWebClient::loadInfo() {
    Preferences preferences;
    preferences.begin("web", false);
    if(preferences.isKey("id") && preferences.isKey("name") && preferences.isKey("token")) {
        id = preferences.getString("id", "");
        name = preferences.getString("name", "");
        accessToken = preferences.getString("token", "");
        credentialsSet = true;
    } else {
        credentialsSet = false;
    }
    preferences.end();
}

bool AGWebClient::sendVerificationCode(const String& verificationCode) {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    http.begin(baseUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);

    String requestBody = "{\"pairCode\": " + verificationCode + "}";
    int httpResponseCode = http.POST(requestBody);

    String payload = "";

    if (httpResponseCode > 0) {
        payload = http.getString();
    } else {
        payload = "Error on sending POST: " + String(httpResponseCode);
    }

    http.end();
    if(deserializeResponse(payload)) return true;
    return false;
}

bool AGWebClient::deserializeResponse(const String& response) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        Serial.print("MQTT credentials deserialization failed: ");
        Serial.println(error.c_str());
        return false;
    }

    if (!doc.containsKey("id") || !doc.containsKey("name") || !doc.containsKey("accessToken") || !doc["mqtt"].containsKey("username") || !doc["mqtt"].containsKey("password")) {
        Serial.println("Invalid credentials: Missing fields.");
        return false;
    }

    id = doc["id"].as<String>();
    name = doc["name"].as<String>();
    accessToken = doc["accessToken"].as<String>();
    mqttUsername = doc["mqtt"]["username"].as<String>();
    mqttPassword = doc["mqtt"]["password"].as<String>();

    saveInfo();
    credentialsSet = true;
    mqttClient.setCredentials(id, name, mqttUsername, mqttPassword);

    return true;
}

bool AGWebClient::pingBackend() {
    if (WiFi.status() != WL_CONNECTED || !mqttClient.isConnected() || !credentialsSet) {
        return false;
    }

    http.begin(baseUrl + "/" + id + "/ping");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", accessToken);
    http.setTimeout(5000);

    int requestCode = http.POST("");
    if(requestCode != 200) {
        Serial.println("Ping failed.");
        return false;
    }

    http.end();

    return true;
}

bool AGWebClient::deleteHub() {
    if (WiFi.status() != WL_CONNECTED || !mqttClient.isConnected() || !credentialsSet) {
        return false;
    }

    http.begin(baseUrl + "/" + id);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", accessToken);
    http.setTimeout(5000);

    int requestCode = http.sendRequest("DELETE", "");
    if(requestCode != 200) {
        Serial.println("Delete failed.");
        return false;
    }
    
    http.end();

    return true;
}
