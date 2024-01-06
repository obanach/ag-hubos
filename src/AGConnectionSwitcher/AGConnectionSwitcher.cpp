// AGConnectionSwitcher.cpp
#include "AGConnectionSwitcher.h"

AGConnectionSwitcher::AGConnectionSwitcher(AGWiFiConnector& connectorRef, AGModuleManager& moduleManagerRef) : connector(connectorRef), moduleManager(moduleManagerRef) {
    carouselRunning = false;
    carouselStartTime = 0;
    carouselMode = 0;
    espTimer = 0;
    discoverTimer = 0;
}

// This function will return 0 or 1 depending on how long the carousel has been running. 0 means WIFI, 1 means ESP-NOW.
int AGConnectionSwitcher::updateCarousel(bool force /* = false */) {
    if (carouselRunning || force) {
        int64_t timeNow = esp_timer_get_time();

        if (carouselMode == 1 && timeNow - espTimer > 4e4 && timeNow - carouselStartTime < 13e4) { // every 40ms but only for 130ms
            espTimer = timeNow;
            Serial.println("Sending package requests...");
            moduleManager.sendPackageRequests();
        }

        if (carouselMode == 1 && timeNow - discoverTimer > 15e4) { // 150ms
            discoverTimer = timeNow;
            Serial.println("Discovering modules...");
            moduleManager.discoverModules();
        }

        if (carouselMode == 0 && timeNow - carouselStartTime > 59e6) { // 59s
            carouselStartTime = timeNow;
            carouselMode = 1;
            Serial.println("It's ESP-NOW-ing time.");
            connector.tempDisconnect();
            espTimer = timeNow;
        }

        if(carouselMode == 1 && timeNow - carouselStartTime > 1.8e5) { // 180ms
            carouselStartTime = timeNow;
            carouselMode = 0;
            Serial.println("It's WiFi-ing time.");
            connector.tempReconnect();
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