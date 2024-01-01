#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "AGWiFiScanner/AGWiFiScanner.h"
#include "AGWiFiConnector/AGWiFiConnector.h"
#include "AGHTMLManager/AGHTMLManager.h"
#include "AGWebServer/AGWebServer.h"
#include "AGModuleManager/AGModuleManager.h"
#include "AGConnectionSwitcher/AGConnectionSwitcher.h"
#include "AGDataManager/AGDataManager.h"

const char* ssid = "AutoGrow - HUB Connection";
const char* password = "autogrow";

AGWiFiScanner scanner;
AGWiFiConnector connector;
AGHTMLManager htmlManager;
AGDataManager dataManager;
AGModuleManager moduleManager(dataManager);
AGConnectionSwitcher connectionSwitcher(connector, moduleManager);
AGWebServer webServer(scanner, connector, htmlManager, connectionSwitcher, moduleManager);

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 69);
DNSServer dnsServer;

int espNowTimer = 0;
bool espNowInitialized = false;

int espWifiNowCarousel = 0;
bool carouselRunning = false;

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
    dnsServer.processNextRequest(); // Redirect all DNS requests to this device's IP address
    connector.updateConnection();
    connectionSwitcher.updateCarousel();
	
	if(!espNowInitialized && connector.isConnected()) {
		moduleManager.initEspNow();
		espNowInitialized = true;
        connectionSwitcher.startCarousel();
	}
}
