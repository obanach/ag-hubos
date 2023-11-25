# AGWiFiScanner Class

## Overview
The `AGWiFiScanner` class is responsible for scanning available Wi-Fi networks and generating HTML content for displaying a dropdown list of the networks. It performs the Wi-Fi scan in a FreeRTOS task to avoid blocking the main loop.

## Class Members
- `scanComplete`: A flag indicating whether the Wi-Fi scan is complete.
- `numberOfNetworks`: The number of networks found during the scan.
- `scanTaskHandle`: A handle to the FreeRTOS task performing the scan.

## Constructor

### AGWiFiScanner()
Constructs an `AGWiFiScanner` object and initializes member variables.

## Methods

### void startScanNetworks()
Initiates a Wi-Fi network scan by creating a FreeRTOS task that calls `scanTask`.

### bool checkScanDone()
Checks if the Wi-Fi scan has been completed.

- **Returns**:
  - `true`: If the scan is complete.
  - `false`: If the scan is still in progress.

### String getNetworksDropdownHTML()
Retrieves the generated HTML dropdown list of Wi-Fi networks.

- **Returns**:
  - A `String` containing the HTML markup for the dropdown.

### String getManualEntryHTML()
Generates an HTML input field for manually entering an SSID.

- **Returns**:
  - A `String` containing the HTML markup for the manual SSID input field.

## Private Methods

### void generateDropdownHTML()
Generates the HTML dropdown list with the results of the Wi-Fi scan.

### static void scanTask(void* parameter)
The static function that runs as a FreeRTOS task to perform the Wi-Fi network scan.

## Usage
To use `AGWiFiScanner`, create an instance of the class and call `startScanNetworks()` to begin scanning. The scan runs asynchronously, and `checkScanDone()` should be called periodically to check for completion. Once the scan is complete, `getNetworksDropdownHTML()` and `getManualEntryHTML()` can be used to get the HTML content for the web interface.

## Note
This class assumes that the ESP32 Wi-Fi module is initialized and that the FreeRTOS scheduler is running.
