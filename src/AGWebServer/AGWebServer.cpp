#include "AGWebServer.h"

AGWebServer::AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef)
: server(80), scanner(scannerRef), connector(connectorRef), htmlManager(htmlManagerRef) {}

void AGWebServer::setupRoutes() {
    // Serve the configuration webpage with the dropdown
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (scanner.scanComplete) {
        request->send(200, "text/html", htmlManager.getWiFiForm(scanner.getNetworksDropdownHTML()));
        } else {
        // Scan is still in progress, inform the client
        request->send(200, "text/html", "Scanning for networks, please refresh in a moment...");
        }
    });

    server.on("/configure", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String ssidValue = request->getParam("ssid", true)->value();
        String passwordValue = request->getParam("password", true)->value();
        connector.attemptConnection(ssidValue, passwordValue);
        request->redirect("/connecting");
        } else {
        request->send(400, "text/html", "<html><body><h1>Missing SSID or password.</h1></body></html>");
        }
    });

    // Serve a page that informs the user that a connection attempt is in progress
    server.on("/connecting", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", connector.getConnectionHTML());
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
        request->send(200, "text/html", "<html><body><h1>Connected successfully!</h1></body></html>");
    });

    server.on("/failure", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<html><body><h1>Failed to connect.</h1><p>Check your credentials and try again.</p></body></html>");
    });

    // Handle not found (if any request comes for a non-existing page)
    server.onNotFound([this](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
}

void AGWebServer::startServer() {
    server.begin();
}
