#ifndef AG_WEBSERVER_H
#define AG_WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include "../AGWiFiScanner/AGWiFiScanner.h"
#include "../AGWiFiConnector/AGWiFiConnector.h"
#include "../AGHTMLManager/AGHTMLManager.h"

class AGWebServer {
public:
    AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef);
    void setupRoutes();
    void startServer();

private:
    AsyncWebServer server;
    AGWiFiScanner& scanner;
    AGWiFiConnector& connector;
    AGHTMLManager& htmlManager;
};

#endif // AGWEBSERVERMANAGER_H
