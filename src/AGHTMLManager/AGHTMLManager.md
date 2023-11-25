# AGHTMLManager Class

## Overview
The `AGHTMLManager` class is responsible for generating HTML content for the ESP32 web server. It provides methods to create HTML forms and other web interface components, which are used to interact with the user for network configuration and status information.

## Constructor

### AGHTMLManager()
Constructs an `AGHTMLManager` object.

## Public Methods

### String getWiFiForm(String dropdownHTML)
Generates an HTML form for the Wi-Fi configuration page.

- **Parameters**:
  - `dropdownHTML`: A `String` containing the HTML markup for the Wi-Fi network dropdown.
- **Returns**:
  - A `String` containing the full HTML for the Wi-Fi configuration form.

## HTML Content

The `getWiFiForm` method generates an HTML document which includes:
- A heading indicating that the ESP32 is acting as an Access Point.
- A paragraph prompting the user to enter the SSID and password for the network they wish to connect to.
- A form that posts to the `/configure` route with two inputs:
  - A dropdown (populated with the provided `dropdownHTML`) for selecting an SSID.
  - A password field for entering the network password.
- A submit button to post the form.
- A button that, when clicked, triggers a refresh of the network list without submitting the form. This is achieved via JavaScript that changes the window location to the `/refresh` route.

## Notes
- The `getWiFiForm` method is designed to be flexible and accepts any valid HTML string to include as the network dropdown, allowing for dynamic updating of the network list.
- The HTML content is returned as a string and can be served directly by the web server in response to HTTP GET requests.
