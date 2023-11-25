#ifndef AG_HTML_MANAGER_H
#define AG_HTML_MANAGER_H

#include <WiFi.h>

class AGHTMLManager {
public:
  AGHTMLManager();
    
  String getWiFiForm(String dropdownHTML);

private:
  String wifiForm;
};

#endif // AG_HTML_MANAGER_H