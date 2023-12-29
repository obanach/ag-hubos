#ifndef AG_WEBSERVER_H
#define AG_WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include "../AGWiFiScanner/AGWiFiScanner.h"
#include "../AGWiFiConnector/AGWiFiConnector.h"
#include "../AGHTMLManager/AGHTMLManager.h"
#include "../AGModuleManager/AGModuleManager.h"
#include "../AGModule/AGModule.h"
#include "../AGUtil/AGUtil.h"
#include "../AGConnectionSwitcher/AGConnectionSwitcher.h"

class AGWebServer {
public:
    AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef, 
                AGConnectionSwitcher& connectionSwitcherRef, AGModuleManager& moduleManagerRef);
    void setupRoutes();
    void startServer();

private:
    AsyncWebServer server;
    AGWiFiScanner& scanner;
    AGWiFiConnector& connector;
    AGHTMLManager& htmlManager;
    AGConnectionSwitcher& connectionSwitcher;
    AGModuleManager& moduleManager;
};

#endif // AGWEBSERVERMANAGER_H
