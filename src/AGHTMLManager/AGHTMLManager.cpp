#include "AGHTMLManager.h"

AGHTMLManager::AGHTMLManager(){};

String AGHTMLManager::getWiFiForm(String dropdownHTML) 
{
    return "<!DOCTYPE html><html><head><title>AutoGrow HUB Connection</title></head><body>"
            "<h1>ESP32 Access Point</h1>"
            "<p>Enter the SSID and password of the network you want the device to connect to.</p>"
            "<form action=\"/configure\" method=\"post\">"
            "<label for=\"ssid\">SSID:</label><br>" + dropdownHTML +
            "<br><label for=\"password\">Password:</label><br>"
            "<input type=\"password\" id=\"password\" name=\"password\"><br><br>"
            "<input type=\"submit\" value=\"Submit\">"
            "</form>"
            "<button type=\"button\" onclick=\"window.location.href='/refresh';\">Refresh Network List</button>"
            "</body></html>";
}
