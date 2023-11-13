




#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

const char* ssid = "AutoGrow - HUB Connection";
const char* password = "admin";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 69);
DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize ESP32 in Access Point mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.println("Access Point Started");

  // Initialize DNS server to redirect all requests to this IP address
  dnsServer.start(DNS_PORT, "*", apIP);

  // Serve the configuration webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<!DOCTYPE html><html><head><title>ESP32 Access Point</title></head><body><h1>ESP32 Access Point</h1><p>Enter the SSID and password of the network you want the device to connect to.</p><form action=\"/configure\" method=\"post\"><label for=\"ssid\">SSID:</label><br><input type=\"text\" id=\"ssid\" name=\"ssid\"><br><label for=\"password\">Password:</label><br><input type=\"password\" id=\"password\" name=\"password\"><br><br><input type=\"submit\" value=\"Submit\"></form></body></html>");
  });

  // Handle Wi-Fi configuration form submit
  server.on("/configure", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String new_ssid = request->getParam("ssid", true)->value();
      String new_password = request->getParam("password", true)->value();
      // Attempt to connect to the new network
      WiFi.begin(new_ssid.c_str(), new_password.c_str());
      
      unsigned long startAttemptTime = millis();
      
      // Blink LED slowly to indicate that the device is trying to connect to the new network
      // TODO: Blink the LED

      // Keep trying to connect for 5 minutes
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 300000) {
        delay(500);
        Serial.print(".");
      }
      
      if(WiFi.status() != WL_CONNECTED) {
        // If connection to the server fails, inform the user
        request->send(500, "text/plain", "Connection to the server failed.");
      } else {
        // If connected, send a ping to the server
        // TODO: Add endpoint connections

        // Inform the user of a successful connection
        request->send(200, "text/plain", "Successfully connected to the network and server.");
      }
    } else {
      request->send(400, "text/plain", "Missing SSID or password.");
    }
  });

  // Handle not found (if any request comes for a non-existing page)
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });

  server.begin();
  Serial.println("Captive portal started");
}

void loop() {
  dnsServer.processNextRequest(); // Redirect all DNS requests to this device's IP address
  
}