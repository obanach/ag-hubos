# AGWebServer Class

## Overview
`AGWebServer` is a class that encapsulates the functionality of an HTTP server for ESP32 devices. It works alongside components for scanning Wi-Fi networks, managing Wi-Fi connections, and generating HTML content. The class sets up an `AsyncWebServer` and configures it with endpoints to interact with users via a web interface.

## Constructor

### AGWebServer(AGWiFiScanner& scannerRef, AGWiFiConnector& connectorRef, AGHTMLManager& htmlManagerRef)
Initializes a new instance of the `AGWebServer` class.

- **Parameters**:
  - `scannerRef`: A reference to an instance of `AGWiFiScanner` for scanning available Wi-Fi networks.
  - `connectorRef`: A reference to an instance of `AGWiFiConnector` for handling Wi-Fi connections.
  - `htmlManagerRef`: A reference to an instance of `AGHTMLManager` for generating HTML content.

## Methods

### void setupRoutes()
Configures the web server routes, including the main configuration page, form submission handling, connection status updates, and static pages for connection results.

### void startServer()
Begins listening for incoming HTTP requests on the server.

## Route Handlers

### `"/"`
Serves the main configuration webpage. If Wi-Fi scanning is complete, it sends an HTML form with a dropdown of available networks. Otherwise, it prompts the user to refresh after a moment.

### `"/configure"`
Handles Wi-Fi configuration form submissions. If parameters are missing, it returns an error message.

### `"/connecting"`
Displays a page to inform the user that a connection attempt is in progress.

### `"/status"`
Provides the current Wi-Fi connection status in JSON format.

### `"/refresh"`
Initiates a new Wi-Fi network scan and redirects to the main page for updated results.

### `"/success"`
Displays a page indicating successful Wi-Fi connection.

### `"/failure"`
Displays a page indicating Wi-Fi connection failure and suggests retrying.

### NotFound Handler
Redirects requests for non-existent pages back to the main configuration page.
