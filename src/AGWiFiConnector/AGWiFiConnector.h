#ifndef AG_WIFI_CONNECTOR_H
#define AG_WIFI_CONNECTOR_H

#include <WiFi.h>
#include <Preferences.h>

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