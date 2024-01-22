#include "AGWebRequest.h"

AGWebRequest::AGWebRequest(String url, String method, String body) {
    this->url = url;
    this->method = method;
    this->body = body;
    this->affectedMacAddress = "";
}

AGWebRequest::AGWebRequest(String url, String method, String body, String affectedMacAddress) {
    this->url = url;
    this->method = method;
    this->body = body;
    this->affectedMacAddress = affectedMacAddress;
}
