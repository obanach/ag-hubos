#include "AGWebServer.h"

AGWebServer::AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef, 
                         AGConnectionSwitcher& connectionSwitcherRef, AGModuleManager& moduleManagerRef)
: server(80), scanner(scannerRef), connector(connectorRef), htmlManager(htmlManagerRef), connectionSwitcher(connectionSwitcherRef), moduleManager(moduleManagerRef) {}

void AGWebServer::setupRoutes() {
    // Serve the configuration webpage with the dropdown
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (connector.isConnected()) {
            request->redirect("/success");
        } else if (scanner.scanComplete) {
            request->send(200, "text/html", htmlManager.getWiFiForm(scanner.getNetworksDropdownHTML()));
        } else {
            request->send(200, "text/html", htmlManager.getScanningPage());
        }
    });

    server.on("/configure", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            String ssidValue = request->getParam("ssid", true)->value();
            String passwordValue = request->getParam("password", true)->value();
            connector.attemptConnection(ssidValue, passwordValue);
            request->redirect("/connecting");
        } else {
            request->send(400, "text/html", htmlManager.getConfigurePage());
        }
    });

    // Serve a page that informs the user that a connection attempt is in progress
    server.on("/connecting", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getConnectingPage());
    });

    // Endpoint to check the connection status
    server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "application/json", connector.getConnectionStatusJSON());
    });
        
    server.on("/refresh", HTTP_GET, [this](AsyncWebServerRequest *request){
        scanner.startScanNetworks();
        request->redirect("/");
    });

    server.on("/success", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getSuccessPage());
    });

    server.on("/failure", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getFailurePage());
    });

    server.on("/reset", HTTP_GET, [this](AsyncWebServerRequest *request){
        connector.resetConnection();
        scanner.startScanNetworks();  
        request->redirect("/");
    });
    
    server.on("/modules", HTTP_GET, [this](AsyncWebServerRequest *request){
        connectionSwitcher.forceEspNow();
        moduleManager.discoverModules();
        request->send(200, "text/html", htmlManager.getModulesPage());
    });

    server.on("/module", HTTP_GET, [this](AsyncWebServerRequest *request){
        connectionSwitcher.forceEspNow();
        if (request->hasParam("mac", false)) {
            Serial.println("Request has a mac param");
            String idValue = request->getParam("mac", false)->value();
            moduleManager.connectModule(idValue);
            connectionSwitcher.startCarousel();
            request->redirect("/"); // todo: change this to a page that shows the module's info
        } else {
            Serial.println("Request doesn't have a mac param");
            connectionSwitcher.startCarousel();
            request->redirect("/"); // todo: change this to a page that shows an error
        }
    });

    // Handle not found (if any request comes for a non-existing page)
    server.onNotFound([this](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
}

void AGWebServer::startServer() {
    server.begin();
}
