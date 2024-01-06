#ifndef AG_WIFI_CONNECTOR_H
#define AG_WIFI_CONNECTOR_H

#include <WiFi.h>
#include <Preferences.h>
#include <esp_wifi.h>

class AGWiFiConnector {
public:
  AGWiFiConnector();
    
  bool checkCredentials();
  void attemptConnection(const String& ssid, const String& password);
  void updateConnection();
  void resetConnection();
  bool isConnected();
  bool isConnectionAttemptFinished();
  String getConnectionStatusJSON();
  String getSavedSSID();
  String getSavedPassword();
  void clearCredentials();
  void tempDisconnect();
  void tempReconnect();

private:
  Preferences wifiCredentials;
  bool shouldAttemptConnection;
  bool connectionAttemptFinished;
  bool connectionSuccessful;
  unsigned long startAttemptTime;
  String ssidToSave;
  String passwordToSave;

  void saveCredentials(const String& ssid, const String& password);
};

#endif // AGWIFI_CONNECTOR_H