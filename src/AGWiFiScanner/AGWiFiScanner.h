#ifndef AG_WIFI_SCANNER_H
#define AG_WIFI_SCANNER_H

#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class AGWiFiScanner {
public:
  int scanComplete;
  int numberOfNetworks;
  TaskHandle_t scanTaskHandle;
  AGWiFiScanner();

  void startScanNetworks();
  bool checkScanDone();
  String getNetworksDropdownHTML();
  String getManualEntryHTML();

private:
  String dropdownHTML;
  void generateDropdownHTML();
  static void scanTask(void* parameter);
};

#endif // AG_WIFI_SCANNER_H
