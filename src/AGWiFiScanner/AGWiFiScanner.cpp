#include "AGWiFiScanner.h"

AGWiFiScanner::AGWiFiScanner() : numberOfNetworks(0), dropdownHTML("") {}

void AGWiFiScanner::scanTask(void *pvParameters) {
  AGWiFiScanner *scanner = static_cast<AGWiFiScanner *>(pvParameters);

  // Clear previous scan results
  scanner->numberOfNetworks = 0;
  scanner->dropdownHTML = "";
  scanner->scanComplete = false;

  // Start WiFi scan
  scanner->numberOfNetworks = WiFi.scanNetworks();
  scanner->scanComplete = true;

  // Check the result of the scan
  if (scanner->numberOfNetworks > 0) {
    // Generate the dropdown HTML
    scanner->generateDropdownHTML();
    Serial.println("Scan complete, " + String(scanner->numberOfNetworks) + " networks found.");
  } else if (scanner->numberOfNetworks == WIFI_SCAN_FAILED) {
    scanner->dropdownHTML = "<p>Scan failed! Please try refreshing the network list and refer to the manual.</p>";
    Serial.println("Scan failed!");
  }

  // Signal the main loop that scan is done
  scanner->scanComplete = true;

  // Delete the task
  scanner->scanTaskHandle = nullptr;
  vTaskDelete(nullptr);
}

void AGWiFiScanner::startScanNetworks() {
  if (scanTaskHandle == nullptr) {
    scanComplete = false;
    xTaskCreate(scanTask, "ScanTask", 4096, this, 1, &scanTaskHandle);
  }
}

bool AGWiFiScanner::checkScanDone() {
  // If scanComplete is true, it means the task has set it and the scan is done
  return scanComplete;
}

void AGWiFiScanner::generateDropdownHTML() {
  Serial.println("Generating dropdown HTML...");
  dropdownHTML = "<select id='networks' name='ssid'>";
  dropdownHTML += "<option value=''>-- Select Network --</option>";

  for (int i = 0; i < numberOfNetworks; ++i) {
    String ssid = WiFi.SSID(i);
    dropdownHTML += "<option value='" + ssid + "'>" + ssid + "</option>";
  }

  dropdownHTML += "</select>";
}

String AGWiFiScanner::getNetworksDropdownHTML() {
  return dropdownHTML;
}

String AGWiFiScanner::getManualEntryHTML() {
  return "<input type='text' id='manual_ssid' name='manual_ssid' placeholder='Enter SSID'>";
}
