#ifndef AG_WIFI_CONNECTOR_H
#define AG_WIFI_CONNECTOR_H

#include <WiFi.h>

class AGWiFiConnector {
public:
  AGWiFiConnector();
    
  void attemptConnection(const String& ssid, const String& password);
  void updateConnection();
  bool isConnected();
  bool isConnectionAttemptFinished();
  String getConnectionStatusJSON();
  String getConnectionHTML();

private:
  bool shouldAttemptConnection;
  bool connectionAttemptFinished;
  bool connectionSuccessful;
  unsigned long startAttemptTime;
};

#endif // AGWIFI_CONNECTOR_H