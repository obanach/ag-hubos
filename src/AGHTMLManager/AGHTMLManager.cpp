#include "AGHTMLManager.h"

AGHTMLManager::AGHTMLManager(){};

String AGHTMLManager::getWiFiFormCSS() {
    return 
        "<style>\n"
        "body {\n"
        "    font-family: 'Roboto', sans-serif;\n"
        "    padding: 20px;\n"
        "    background-color: #f5f5f5;\n"
        "}\n"
        ".container {\n"
        "    background-color: white;\n"
        "    padding: 20px;\n"
        "    border-radius: 4px;\n"
        "    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\n"
        "    max-width: 500px;\n"
        "    margin: auto;\n"
        "}\n"
        ".form-group {\n"
        "    margin-bottom: 15px;\n"
        "}\n"
        ".form-group label {\n"
        "    display: block;\n"
        "    margin-bottom: 5px;\n"
        "}\n"
        ".form-control {\n"
        "    width: 100%;\n"
        "    padding: 10px;\n"
        "    border: 1px solid #ccc;\n"
        "    border-radius: 4px;\n"
        "    box-sizing: border-box;\n"
        "}\n"
        ".btn {\n"
        "    padding: 10px 15px;\n"
        "    border: none;\n"
        "    border-radius: 4px;\n"
        "    cursor: pointer;\n"
        "    font-size: 16px;\n"
        "    text-transform: uppercase;\n"
        "}\n"
        ".btn-primary {\n"
        "    background-color: #6200ea;\n"
        "    color: white;\n"
        "}\n"
        ".btn-secondary {\n"
        "    margin-top: 8px;\n"
        "    background-color: #e0e0e0;\n"
        "    color: black;\n"
        "}\n"
        ".dropdown select {\n"
        "    width: 100%;\n"
        "    padding: 10px;\n"
        "    border: 1px solid #ccc;\n"
        "    border-radius: 4px;\n"
        "    background-color: white;\n"
        "    box-sizing: border-box;\n"
        "    font-size: 16px;\n"
        "}\n"
        ".dropdown select:focus {\n"
        "    outline: none;\n"
        "    border-color: #6200ea;\n"
        "}\n"
        "</style>\n";
}

String AGHTMLManager::getWiFiForm(String dropdownHTML) 
{  
    return "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>AutoGrow HUB Connection</title>\n"
        + getWiFiFormCSS() +
        "</head>\n"
        "<body>\n"
        "<div class='container'>\n"
        "    <h1>AutoGrow HUB Connection</h1>\n"
        "    <p>Enter the SSID and password of the network you want the device to connect to.</p>\n"
        "    <form action='/configure' method='post'>\n"
        "        <div class='form-group'>\n"
        "            <label for='ssid'>SSID:</label>\n"
        "            <div class='dropdown'>\n"
                        + dropdownHTML +
        "            </div>\n"
        "        </div>\n"
        "        <div class='form-group'>\n"
        "            <label for='password'>Password:</label>\n"
        "            <input type='password' id='password' name='password' class='form-control'>\n"
        "        </div>\n"
        "        <button type='submit' class='btn btn-primary'>Submit</button>\n"
        "    </form>\n"
        "    <button type='button' class='btn btn-secondary' onclick=\"window.location.href='/refresh';\">Refresh Network List</button>\n"
        "</div>\n"
        "</body>\n"
        "</html>\n";
}

String AGHTMLManager::getConfigurePage() {
    return "<!DOCTYPE html><html><head><title>Error</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        "h1 { color: #d32f2f; }"
        "p { color: #666; }"
        "</style>"
        "</head><body>"
        "<h1>Missing SSID or Password</h1>"
        "<p>Please go back and make sure to fill both the SSID and password fields.</p>"
        "</body></html>";
}

String AGHTMLManager::getConnectingPage() {
    return "<!DOCTYPE html><html><head><title>Connecting...</title>"
       "<style>"
       "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
       "h1 { color: #1976d2; }"
       "</style>"
       "</head><body>"
       "<h1>Connecting...</h1>"
       "<script>"
       "setInterval(function() {"
       "    fetch('/status').then(response => response.json()).then(data => {"
       "        if (data.connected) {"
       "            window.location.href = '/success';"
       "        } else if (data.failed) {"
       "            window.location.href = '/failure';"
       "        }"
       "    });"
       "}, 5000);"
       "</script>"
       "</body></html>";
}

String AGHTMLManager::getFailurePage() {
    return "<!DOCTYPE html><html><head><title>Failure</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        "h1 { color: #d32f2f; }"
        "p { color: #666; }"
        "</style>"
        "</head><body>"
        "<h1>Failed to Connect</h1>"
        "<p>Check your credentials and try again.</p>"
        "</body></html>";
}

String AGHTMLManager::getSuccessPage() {
    return "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Success</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        "h1 { color: #388e3c; }"
        ".button {"
        "  display: inline-block;"
        "  margin: 0px 10px;"
        "  padding: 10px 20px;"
        "  font-size: 16px;"
        "  cursor: pointer;"
        "  text-align: center;"
        "  text-decoration: none;"
        "  outline: none;"
        "  color: #fff;"
        "  background-color: #ff483b;"
        "  border: none;"
        "  border-radius: 15px;"
        "  box-shadow: 0 9px #999;"
        "}"
        ".button-blue { background-color: #2196f3; }"
        ".button:hover { background-color: #ff3021; }"
        ".button:active {"
        "  background-color: #ff1303;"
        "  box-shadow: 0 5px #666;"
        "  transform: translateY(4px);"
        "}"
        ".button-blue:hover { background-color: #0b7dda; }"
        ".button-blue:active { background-color: #0a6cd1; }"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Connected Successfully!</h1>"
        "<a href='/modules' class='button button-blue'>Add a module</a>"
        "<a href='/reset' class='button'>Reset</a>"
        "</body>"
        "</html>";
}

String AGHTMLManager::getScanningPage() {
    return "<!DOCTYPE html><html><head><title>Scanning</title>"
        "<script>"
        "    window.onload = function() {"
        "        setTimeout(function() {"
        "            window.location.reload();"
        "        }, 5000);"
        "    };"
        "</script>"
        "<style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        "h1 { color: #ffa000; }"
        "p { color: #666; }"
        "</style>"
        "</head><body>"
        "<h1>Scanning for Networks</h1>"
        "<p>The page will refresh automatically. If that doesn't happen, please refresh in a moment.</p>"
        "</body></html>";
}

String AGHTMLManager::getModulesPage() {
    String html = "<!DOCTYPE html><html><head><title>Modules</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        ".module { background-color: #FFFFFF; border: 1px solid #DDD; padding: 10px; margin-bottom: 10px; border-radius: 4px; cursor: pointer; }"
        ".module:hover { background-color: #EEE; }"
        "h1 { color: #333; }"
        "h2 { color: #444; }"
        "h3 { color: #555; }"
        "button { padding: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; }"
        "button:hover { background-color: #45a049; }"
        "</style>"
        "</head><body>"
        "<h1>Available Modules</h1>";

    std::vector<AGModule> modules = AGModuleManager::instance->getDiscoveredModuleList();
    std::vector<AGModule> connectedModules = AGModuleManager::instance->getConnectedModuleList();

    if(modules.size() == 0 && connectedModules.size() == 0) {
        html += "<h2>No modules found :(</h2>"
        "<h3>Please refresh the page in a moment.</h3>"
        "<h3>If the issue persists, please make sure that the module you'd like to add is powered on an in range.</h3>";
    } else {
        // Display Connected and Nearby Modules
        html += "<h2>Connected and working:</h2>";
        for (const auto& module : modules) {
            for (const auto& connectedModule : connectedModules) {
                if (module.isMacAddressEqual(connectedModule.macAddress)) {
                    html += "<div class='module' onclick=\"window.location.href='module?mac=" + macToString(module.macAddress) + "'\">"
                        "<h2>" + module.name + " (" + module.type + ")</h2>"
                        "<p>MAC: " + macToString(module.macAddress) + "</p>"
                        "</div>";
                    break;
                }
            }
        }

        // Display Connected, but Not Nearby Modules
        html += "<h2>Connected, but unreachable:</h2>";
        for (const auto& connectedModule : connectedModules) {
            bool isNearby = false;
            for (const auto& module : modules) {
                if (connectedModule.isMacAddressEqual(module.macAddress)) {
                    isNearby = true;
                    break;
                }
            }
            if (!isNearby) {
                html += "<div class='module' onclick=\"window.location.href='module?mac=" + macToString(connectedModule.macAddress) + "'\">"
                    "<h2>" + connectedModule.name + " (" + connectedModule.type + ")</h2>"
                    "<p>MAC: " + macToString(connectedModule.macAddress) + "</p>"
                    "</div>";
            }
        }

        // Display Not Connected Modules
        html += "<h2>Not connected:</h2>";
        for (const auto& module : modules) {
            bool isConnected = false;
            for (const auto& connectedModule : connectedModules) {
                if (module.isMacAddressEqual(connectedModule.macAddress)) {
                    isConnected = true;
                    break;
                }
            }
            if (!isConnected) {
                html += "<div class='module' onclick=\"window.location.href='module?mac=" + macToString(module.macAddress) + "'\">"
                    "<h2>" + module.name + " (" + module.type + ")</h2>"
                    "<p>MAC: " + macToString(module.macAddress) + "</p>"
                    "</div>";
            }
        }
    }

    // Adding the 'Done' button
    html += "<button onclick=\"window.location.href='modulesDone'\">Done</button>";

    html += "</body></html>";

    return html;
}
