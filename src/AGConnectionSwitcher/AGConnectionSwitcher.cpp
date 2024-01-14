// AGConnectionSwitcher.cpp
#include "AGConnectionSwitcher.h"
#include "AGModuleManager/AGModuleManager.h"

AGConnectionSwitcher::AGConnectionSwitcher(AGWiFiConnector& connectorRef, AGMQTTClient& mqttClientRef, AGWebClient& webClientRef) 
: connector(connectorRef), mqttClient(mqttClientRef), moduleManager(nullptr), webClient(webClientRef) {
    carouselRunning = false;
    carouselStartTime = 0;
    carouselMode = 0;
    espTimer = 0;
    discoverTimer = 0;
    espNowTimedForce = false;
}

// This function will return 0 or 1 depending on how long the carousel has been running. 0 means WIFI, 1 means ESP-NOW.
int AGConnectionSwitcher::updateCarousel(bool force /* = false */) {
    if (espNowTimedForce) {
        int64_t timeNow = esp_timer_get_time();
        if(timeNow - espTimer > espNowTimedForceTime) {
            espNowTimedForce = false;
            Serial.println("ESP-NOW timed force expired.");
            connector.tempReconnect();
            mqttClient.tempReconnect();
            carouselStartTime = timeNow;
        }
    } else if (carouselRunning || force) {
        int64_t timeNow = esp_timer_get_time();

        if (carouselMode == 1 && timeNow - espTimer > 4e4 && timeNow - carouselStartTime < 18e4) { // every 40ms but only for 180ms
            espTimer = timeNow;
            Serial.println("Sending package requests...");
            moduleManager->sendPackageRequests();
        }

        if (carouselMode == 1 && timeNow - discoverTimer > 15e4) { // 150ms
            discoverTimer = timeNow;
            Serial.println("Discovering modules...");
            moduleManager->discoverModules();
        }

        if (carouselMode == 0 && timeNow - carouselStartTime > 59e6) { // 59s
            carouselMode = 1;
            Serial.println("It's ESP-NOW-ing time.");
            mqttClient.tempDisconnect();
            connector.tempDisconnect();
            carouselStartTime = timeNow;
            espTimer = timeNow;
        }

        if(carouselMode == 1 && timeNow - carouselStartTime > 2.3e5) { // 230ms
            carouselStartTime = timeNow;
            carouselMode = 0;
            Serial.println("It's WiFi-ing time.");
            mqttClient.connecting = true;
            connector.tempReconnect();
            mqttClient.tempReconnect();
            webClient.pingBackend();
        }
    }
    return carouselMode;
}

void AGConnectionSwitcher::resetCarousel() {
    Serial.println("Resetting carousel...");
    carouselMode = 0;
}

void AGConnectionSwitcher::startCarousel() {
    Serial.println("Starting carousel...");
    if (carouselRunning) return;
    carouselRunning = true;
    carouselStartTime = esp_timer_get_time() / 1000;
}

void AGConnectionSwitcher::stopCarousel() {
    Serial.println("Stopping carousel...");
    carouselRunning = false;
}

bool AGConnectionSwitcher::isCarouselRunning() {
    return carouselRunning;
}

void AGConnectionSwitcher::forceWiFi() {
    Serial.println("Forcing WiFi...");
    carouselRunning = false;
    carouselMode = 0;
    connector.tempReconnect();
}

void AGConnectionSwitcher::forceEspNow() {
    Serial.println("Forcing ESP-NOW...");
    carouselRunning = false;
    carouselMode = 1;
    connector.tempDisconnect();
}

void AGConnectionSwitcher::forceEspNow(int timeToForce) {
    Serial.println("Forcing ESP-NOW for " + String(timeToForce) + "ms...");
    espNowTimedForce = true;
    espNowTimedForceTime = timeToForce;
    carouselRunning = false;
    carouselMode = 1;
    connector.tempDisconnect();
}

void AGConnectionSwitcher::setModuleManager(AGModuleManager* moduleManager) {
    this->moduleManager = moduleManager;
}
