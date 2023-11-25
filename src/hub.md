# AutoGrow HUB Documentation

## Overview
This ESP32 server application sets up an ESP32 device as an Access Point with an HTTP server. It is designed to scan for Wi-Fi networks, enable Wi-Fi connections, and serve web pages to configure Wi-Fi settings through a web interface.

## Components
- `AGWiFiScanner`: Scans for available Wi-Fi networks.
- `AGWiFiConnector`: Manages Wi-Fi connections.
- `AGHTMLManager`: Generates HTML content for the server.
- `AGWebServer`: Configures and manages the web server routes and operations.
- `DNSServer`: Captive portal functionality to redirect all DNS requests to the ESP32's IP address.

## Setup

### Access Point Configuration
The ESP32 is configured to operate in Access Point mode with a predefined SSID and password. It starts a DNS server to handle all DNS requests by redirecting them to the ESP32's IP address.

### Web Server Initialization
The `AGWebServer` instance is set up with routes for:
- Serving the main configuration page.
- Handling Wi-Fi configuration form submissions.
- Displaying connection status.
- Refreshing the list of Wi-Fi networks.

### Wi-Fi Network Scanning
A scan for available Wi-Fi networks is initiated at setup.

## Loop Behavior

### DNS Request Handling
The `dnsServer` continuously processes incoming DNS requests to ensure that clients can navigate to the ESP32's web server correctly.

### Connection Status Updating
The `connector` checks and updates the Wi-Fi connection status. If the connection attempt has finished, it logs the outcome to the serial console.

## Connection Attempt Handling
When a connection attempt is finished, the application checks whether it was successful:
- If connected, it prints a success message to the serial console.
- If failed, it prints a failure message to the serial console.
