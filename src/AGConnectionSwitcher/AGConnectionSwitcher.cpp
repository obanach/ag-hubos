// AGConnectionSwitcher.cpp
#include "AGConnectionSwitcher.h"

AGConnectionSwitcher::AGConnectionSwitcher(AGWiFiConnector& connectorRef, AGModuleManager& moduleManagerRef) : connector(connectorRef), moduleManager(moduleManagerRef) {
    carouselRunning = false;
    carouselStartTime = 0;
    carouselMode = 0;
    espTimer = 0;
}

// This function will return 0 or 1 depending on how long the carousel has been running. 0 means WIFI, 1 means ESP-NOW.
int AGConnectionSwitcher::updateCarousel(bool force /* = false */) {
    if (carouselRunning || force) {
        int64_t timeNow = esp_timer_get_time();

        if (carouselMode == 1 && timeNow - espTimer > 5e6) {
            espTimer = timeNow;
            Serial.println("Discovering modules...");
            moduleManager.discoverModules();
            Serial.println("Sending package requests...");
            moduleManager.sendPackageRequests();
        }

        if (timeNow - carouselStartTime > 30e6) {
            carouselStartTime = timeNow;
            carouselMode++;
            if (carouselMode >= 2) carouselMode = 0;

            if (carouselMode == 0) {
                Serial.println("It's WiFi-ing time.");
                connector.tempReconnect();
            } else {
                Serial.println("It's ESP-NOW-ing time.");
                connector.tempDisconnect();
                espTimer = timeNow;
            }
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