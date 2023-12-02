#ifndef AG_HTML_MANAGER_H
#define AG_HTML_MANAGER_H

#include <WiFi.h>

class AGHTMLManager {
public:
  AGHTMLManager();
    
  String getWiFiForm(String dropdownHTML);
  String getWiFiFormCSS();
  String getConfigurePage();
  String getConnectingPage();
  String getSuccessPage();
  String getFailurePage();
  String getScanningPage();
};

#endif // AG_HTML_MANAGER_H