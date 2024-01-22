#ifndef AG_WEB_REQUEST_H
#define AG_WEB_REQUEST_H

#include <Arduino.h>
#include <WiFi.h>

class AGWebRequest {
public:
    AGWebRequest(String url, String method, String body);
    AGWebRequest(String url, String method, String body, String affectedMacAddress);
    String url;
    String method;
    String body;
    String affectedMacAddress;
    int retries = 0;
};

#endif // AG_WEB_REQUEST_H