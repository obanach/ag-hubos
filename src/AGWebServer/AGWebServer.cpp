#include "AGWebServer.h"

AGWebServer::AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef, 
                         AGConnectionSwitcher& connectionSwitcherRef, AGModuleManager& moduleManagerRef, AGWebClient& webClientRef, AGMQTTClient& mqttClientRef)
: server(80), scanner(scannerRef), connector(connectorRef), htmlManager(htmlManagerRef), 
connectionSwitcher(connectionSwitcherRef), moduleManager(moduleManagerRef), webClient(webClientRef), mqttClient(mqttClientRef) {}

void AGWebServer::setupRoutes() {
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (connector.isConnected()) {
            if(mqttClient.isConnected()) {
                request->redirect("/pairSuccess");
            } else {
                request->redirect("/code");
            }
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

    server.on("/connecting", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getConnectingPage());
    });

    server.on("/refresh", HTTP_GET, [this](AsyncWebServerRequest *request){
        scanner.startScanNetworks();
        request->redirect("/");
    });

    server.on("/success", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->redirect("/code");
    });

    server.on("/failure", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getFailurePage());
    });

    server.on("/reset", HTTP_GET, [this](AsyncWebServerRequest *request){
        codeCorrect = -1;
        mqttClient.resetConnection();
        Serial.println("MQTT reset.");
        webClient.deleteHub();
        Serial.println("Hub deleted.");
        moduleManager.clearModules();
        Serial.println("Modules cleared.");
        connector.resetConnection();
        Serial.println("WiFi reset.");
        scanner.startScanNetworks();
        Serial.println("Scanning started.");
        request->redirect("/");
    });

    server.on("/code", HTTP_GET, [this](AsyncWebServerRequest *request){
        if(codeCorrect == 1)
            request->redirect("/pairSuccess");
        else if(codeCorrect == 0)
            request->redirect("/pairFailure");
        else if(codeCorrect == -1)
            request->send(200, "text/html", htmlManager.getCodeInputPage());
    });

    server.on("/submitCode", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("code", true)) {
            String codeValue = request->getParam("code", true)->value();
            Serial.println("Code submitted: " + codeValue);
            codeCorrect = -1;
            if(webClient.sendVerificationCode(codeValue)) {
                codeCorrect = 1;
                request->redirect("/pairSuccess");
            } else {
                codeCorrect = 0;
                request->redirect("/pairFailure");
            }
        } else {
            request->send(400, "text/html", htmlManager.getCodeInputPage());
        }
    });

    server.on("/pairSuccess", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlManager.getPairingSuccessPage());
    });

    server.on("/pairFailure", HTTP_GET, [this](AsyncWebServerRequest *request){
        if(codeCorrect == 0 || codeCorrect == -1)
            request->send(200, "text/html", htmlManager.getPairingFailurePage());
        else
            request->redirect("/code");
    });

    server.onNotFound([this](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
}

void AGWebServer::startServer() {
    server.begin();
}
