#ifndef AG_HTML_MANAGER_H
#define AG_HTML_MANAGER_H

#include <WiFi.h>
#include "AGModuleManager/AGModuleManager.h"
#include "AGModule/AGModule.h"
#include "AGUtil/AGUtil.h"

class AGHTMLManager {
public:
  AGHTMLManager();
    
  String getWiFiForm(String dropdownHTML);
  String getWiFiFormCSS();
  String getConfigurePage();
  String getConnectingPage();
  String getFailurePage();
  String getScanningPage();
  String getModulesPage();
  String getCodeInputPage();
  String getPairingSuccessPage();
  String getPairingFailurePage();
};

#endif // AG_HTML_MANAGER_H