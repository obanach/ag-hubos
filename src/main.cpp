#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "AGWiFiScanner/AGWiFiScanner.h"
#include "AGWiFiConnector/AGWiFiConnector.h"
#include "AGHTMLManager/AGHTMLManager.h"
#include "AGWebServer/AGWebServer.h"

const char* ssid = "AutoGrow - HUB Connection";
const char* password = "autogrow";

AGWiFiScanner scanner;
AGWiFiConnector connector;
AGHTMLManager htmlManager;
AGWebServer webServer(scanner, connector, htmlManager);

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 69);
DNSServer dnsServer;

void setup() {
  Serial.begin(115200);

  // Initialize ESP32 in Access Point mode
  WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.println("Access Point Started");

  // Initialize DNS server to redirect all requests to this IP address
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.setupRoutes();
  webServer.startServer();

  scanner.startScanNetworks();
  Serial.println("Captive portal started");
}

void loop() {
  dnsServer.processNextRequest(); // Redirect all DNS requests to this device's IP address
  connector.updateConnection();

  if (connector.isConnectionAttemptFinished()) {
    if (connector.isConnected()) {
      // Handle successful connection
      Serial.println("Connected to WiFi successfully.");
    } else {
      // Handle failed connection
      Serial.println("Connection to WiFi failed.");
    }
  }
}
