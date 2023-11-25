# AGWiFiConnector Class

## Overview
The `AGWiFiConnector` class is designed to manage Wi-Fi connections on an ESP32 device. It handles the process of connecting to a Wi-Fi network with provided SSID and password credentials and provides status updates through JSON and HTML formats.

## Constructor

### AGWiFiConnector()
Initializes a new instance of the `AGWiFiConnector` class.

## Public Methods

### void attemptConnection(const String& ssid, const String& password)
Begins an attempt to connect to a Wi-Fi network with the specified SSID and password.

- **Parameters**:
  - `ssid`: The SSID of the Wi-Fi network to connect to.
  - `password`: The password of the Wi-Fi network.

### void updateConnection()
Monitors the status of the Wi-Fi connection attempt and updates internal state accordingly.

### bool isConnected()
Checks if the device is connected to a Wi-Fi network.

- **Returns**:
  - `true`: If the device is connected.
  - `false`: Otherwise.

### bool isConnectionAttemptFinished()
Checks if the connection attempt has finished.

- **Returns**:
  - `true`: If the connection attempt is finished.
  - `false`: Otherwise.

### String getConnectionStatusJSON()
Provides a JSON representation of the connection status.

- **Returns**:
  - A `String` containing a JSON object with the connection status.

### String getConnectionHTML()
Generates an HTML page that continuously checks the connection status and redirects based on the result.

- **Returns**:
  - A `String` containing the HTML markup for the connection status page.

## Private Members
- `shouldAttemptConnection`: Indicates whether a connection attempt should be made.
- `connectionAttemptFinished`: Indicates whether the connection attempt has finished.
- `connectionSuccessful`: Indicates whether the connection attempt was successful.
- `startAttemptTime`: The timestamp marking the start of the connection attempt.

## Usage
Create an instance of `AGWiFiConnector` and use `attemptConnection()` to initiate a Wi-Fi connection. Call `updateConnection()` regularly to process the connection attempt. Use `isConnected()` and `isConnectionAttemptFinished()` to query the connection status. The methods `getConnectionStatusJSON()` and `getConnectionHTML()` can be used to provide feedback to a web interface.

## Note
The class assumes the ESP32 Wi-Fi module is properly initialized before attempting to connect.
