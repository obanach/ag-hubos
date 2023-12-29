#ifndef AG_CONNECTION_SWITCHER_H
#define AG_CONNECTION_SWITCHER_H

#include <WiFi.h>
#include <Preferences.h>
#include "AGWiFiConnector/AGWiFiConnector.h"
#include "AGModuleManager/AGModuleManager.h"
#include "AGDataManager/AGDataManager.h"

class AGConnectionSwitcher {
public:
    AGConnectionSwitcher(AGWiFiConnector& connector, AGModuleManager& moduleManager);
    int updateCarousel(bool force = false);
    void resetCarousel();
    void startCarousel();
    void stopCarousel();
    bool isCarouselRunning();
    void forceEspNow();
    void forceWiFi();

private:
    bool carouselRunning;
    int64_t carouselStartTime;
    int64_t espTimer;
    int carouselMode;
    AGWiFiConnector& connector;
    AGModuleManager& moduleManager;
};

#endif // AG_CONNECTION_SWITCHER_H