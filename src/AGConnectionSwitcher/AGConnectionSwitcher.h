#ifndef AG_CONNECTION_SWITCHER_H
#define AG_CONNECTION_SWITCHER_H

#include <WiFi.h>
#include <Preferences.h>
#include "AGWiFiConnector/AGWiFiConnector.h"
#include "AGDataManager/AGDataManager.h"
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGWebClient/AGWebClient.h"

class AGModuleManager;

class AGConnectionSwitcher {
public:
    AGConnectionSwitcher(AGWiFiConnector& connectorRef, AGMQTTClient& mqttClientRef, AGWebClient& webClientRef);
    int updateCarousel(bool force = false);
    void resetCarousel();
    void startCarousel();
    void stopCarousel();
    bool isCarouselRunning();
    void forceEspNow();
    void forceEspNow(int timeToForce, bool forceDiscoverModules = false);
    void forceWiFi();
    void setModuleManager(AGModuleManager* moduleManager);

private:
    bool carouselRunning;
    bool espNowTimedForce;
    bool forcedDiscovery;
    int espNowTimedForceTime;
    int64_t carouselStartTime;
    int64_t espTimer;
    int64_t discoverTimer;
    int carouselMode;
    AGWiFiConnector& connector;
    AGModuleManager* moduleManager;
    AGMQTTClient& mqttClient;
    AGWebClient& webClient;
};

#endif // AG_CONNECTION_SWITCHER_H