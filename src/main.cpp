#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "AGWiFiScanner/AGWiFiScanner.h"
#include "AGWiFiConnector/AGWiFiConnector.h"
#include "AGHTMLManager/AGHTMLManager.h"
#include "AGWebServer/AGWebServer.h"
#include "AGModuleManager/AGModuleManager.h"
#include "AGConnectionSwitcher/AGConnectionSwitcher.h"
#include "AGDataManager/AGDataManager.h"
#include "AGMQTTClient/AGMQTTClient.h"
#include "AGWebClient/AGWebClient.h"

const char* ssid = "AutoGrow - HUB Connection";
const char* password = "autogrow";
IPAddress apIP(192, 168, 0, 69);

AGWiFiScanner scanner;
AGWiFiConnector connector;
AGHTMLManager htmlManager;
AGMQTTClient mqttClient;
AGWebClient webClient(mqttClient);
AGDataManager dataManager(mqttClient, webClient);
AGModuleManager moduleManager(dataManager);
AGConnectionSwitcher connectionSwitcher(connector, mqttClient, webClient);
AGWebServer webServer(scanner, connector, htmlManager, connectionSwitcher, moduleManager, webClient, mqttClient);

const byte DNS_PORT = 53;
DNSServer dnsServer;

int espNowTimer = 0;
bool espNowInitialized = false;

int espWifiNowCarousel = 0;
bool carouselRunning = false;

unsigned long mqttPackageMillis = 0;

void setup() {
    Serial.begin(115200);

    // Initialize ESP32 in Access Point mode
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    Serial.println("Access Point Started");

    // Initialize DNS server to redirect all requests to this IP address
    dnsServer.start(DNS_PORT, "*", apIP);

    webServer.setupRoutes();
    webServer.startServer();

    webClient.loadInfo();
    webClient.setModuleManager(&moduleManager);
    mqttClient.loadCredentials();
    moduleManager.setMQTTClient(&mqttClient);
    moduleManager.setConnectionSwitcher(&connectionSwitcher);
    moduleManager.setWebClient(&webClient);
    mqttClient.setModuleManager(&moduleManager);
    connectionSwitcher.setModuleManager(&moduleManager);

    espNowInitialized = false;

    if(connector.checkCredentials()) { // TODO: move to connector
        Serial.println("Saved credentials found, attempting connection...");
        connector.attemptConnection(connector.getSavedSSID(), connector.getSavedPassword());
    } else {
        Serial.println("No saved credentials found.");
        scanner.startScanNetworks();  
    }

    moduleManager.loadModulesFromMemory();
    Serial.println("Captive portal started");
}

void loop() {
    dnsServer.processNextRequest();
    connector.updateConnection();
    connectionSwitcher.updateCarousel();
	
	if(!espNowInitialized && connector.isConnected()) {
		moduleManager.initEspNow();
		espNowInitialized = true;
        connectionSwitcher.startCarousel();
	}

    if(connector.isConnected() && !mqttClient.isConnected()) {
        mqttClient.begin();
    }

    if(connector.isConnected() && mqttClient.isConnected()) {
        mqttClient.loop();
        webClient.loop();
    }

    if(webClient.resetFlag) {
        webClient.resetFlag = false;
        webClient.credentialsSet = false;
        moduleManager.clearModules();
        delay(500);
        webClient.saveInfo();
        delay(500);
        mqttClient.resetConnection();
        Serial.println("Resetting connection...");
    }
    
    unsigned long currentMillis = millis();
    if(currentMillis - mqttPackageMillis > 3000) {
        mqttPackageMillis = currentMillis;
        if(connector.isConnected() && mqttClient.isConnected()) {
            dataManager.sendStoredPackage();
        } else Serial.println("Not connected to WiFi or MQTT broker, skipping sending stored package.");
    }
}
