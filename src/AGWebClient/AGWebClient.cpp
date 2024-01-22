#include "AGWebClient.h"
#include "AGModuleManager/AGModuleManager.h"

String getIDFromPayload(const String& payload) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.println(error.c_str());
        return "";
    }

    if (!doc.containsKey("id")) {
        Serial.println("Invalid payload: missing ID.");
        return "";
    }

    return doc["id"].as<String>();
}

AGWebClient::AGWebClient(AGMQTTClient& mqttClientRef) : mqttClient(mqttClientRef), moduleManager(nullptr) {}

void AGWebClient::loop() {
    if (storedRequests.size() == 0 || WiFi.status() != WL_CONNECTED || !mqttClient.isConnected()) {
        return;
    }

    for(int i = 0; i < storedRequests.size(); i++) {
        if(storedRequests[i].method == "POST") {
            Serial.println("Sending POST request to " + baseUrl + "hub/" + id + storedRequests[i].url + " with body " + storedRequests[i].body + " and token " + accessToken + ".");
            http.begin(baseUrl + "hub/" + id + storedRequests[i].url);
            http.addHeader("Content-Type", "application/json");
            http.addHeader("X-Device-Token", accessToken);
            http.setTimeout(10000);
            storedRequests[i].retries++;

            if(storedRequests[i].retries > 5) {
                Serial.println("Request failed 5 times. Aborting.");
                continue;
            }

            int requestCode = http.POST(storedRequests[i].body);
            if(requestCode != 200) {
                Serial.println("Request failed with code " + String(requestCode) + ".");
                continue;
            } else if(requestCode == 403) {
                Serial.println("Unauthorized.");
                resetFlag = true;
                continue;
            }

            if(storedRequests[i].url == "/module") {
                String payload = "";
                if (requestCode > 0) {
                    delay(1000);
                    payload = http.getString();
                    Serial.println("Got payload: " + payload);
                } else {
                    payload = "Error on sending POST: " + String(requestCode);
                }

                String id = getIDFromPayload(payload);
                http.end();
                if(moduleManager->setModuleID(storedRequests[i].affectedMacAddress, id)) {
                    Serial.println("Module ID set: " + id + ".");
                    DynamicJsonDocument doc(1024);
                    doc["paired"] = true;
                    doc["macAddress"] = storedRequests[i].affectedMacAddress;
                    doc["id"] = id.toInt();
                    String output;
                    serializeJson(doc, output);
                    AGServerPacket serverPacket;
                    serverPacket.topic = mqttClient.topic + "/module/pair";
                    serverPacket.contents = output;
                    mqttClient.publish(serverPacket);
                } else {
                    Serial.println("Module ID not set.");
                }
            }
        }
    }

    storedRequests.clear();
}

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

    http.begin(baseUrl + "/hub/pair");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(8000);

    String requestBody = "{\"pairCode\": " + verificationCode + "}";
    int httpResponseCode = http.POST(requestBody);

    String payload = "";

    if (httpResponseCode > 0) {
        delay(1000);
        payload = http.getString();
        Serial.println("Got payload: " + payload);
    } else {
        payload = "Error on sending POST: " + String(httpResponseCode);
    }

    Serial.println("Got payload: " + payload);

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

    credentialsSet = true;
    saveInfo();
    mqttClient.setCredentials(id, name, mqttUsername, mqttPassword);

    return true;
}

bool AGWebClient::pingBackend() {
    if (WiFi.status() != WL_CONNECTED || !mqttClient.isConnected() || !credentialsSet) {
        AGWebRequest request("/ping", "POST", "", "");
        storeRequest(request);
        return false;
    }

    http.begin(baseUrl + "/hub/" + id + "/ping");
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

    http.begin(baseUrl + "/hub/" + id);
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

String AGWebClient::registerModule(const AGModule module) {
    if (WiFi.status() != WL_CONNECTED || !mqttClient.isConnected() || !credentialsSet) {
        Serial.println("Registering the module failed. Not connected to the internet or MQTT.");
        AGWebRequest request("/module", "POST", "{\"name\": \"" + module.name + "\", \"type\": \"" + module.type + "\", \"macAddress\": \"" + macToString(module.macAddress) + "\"}", macToString(module.macAddress));
        storeRequest(request);
        return "";
    }

    http.begin(baseUrl + "/hub/" + id + "/module");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", accessToken);
    http.setTimeout(5000);

    String requestBody = "{\"name\": \"" + module.name + "\", \"type\": \"" + module.type + "\", \"macAddress\": \"" + macToString(module.macAddress) + "\"}";
    int requestCode = http.POST(requestBody);
    if(requestCode != 200) {
        Serial.println("Registering the module failed.");
        return "";
    }

    String payload = "";
    if (requestCode > 0) {
        delay(1000);
        payload = http.getString();
        Serial.println("Got payload: " + payload);
    } else {
        payload = "Error on sending POST: " + String(requestCode);
    }

    String id = getIDFromPayload(payload);
    http.end();

    Serial.println("Module registered with ID: " + id);
    return id;
}

void AGWebClient::storeRequest(AGWebRequest request) {
    storedRequests.push_back(request);
}

void AGWebClient::setModuleManager(AGModuleManager* moduleManager) {
    this->moduleManager = moduleManager;
}
