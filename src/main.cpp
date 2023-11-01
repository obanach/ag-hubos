#include <Arduino.h>
#include <Log.h>
#include <LedManager.h>
#include <WiFiManager.h>

const String VERSION = "0.1";
const int LED_PIN = 2; // change this to the pin connected to your LED

LedManager led(LED_PIN);

void setup() {
  Serial.begin(115200); // initialize serial communication with baud rate of 9600
  String message = "AG-HubOS v" + VERSION + " started";
  logger.info(message.c_str());
  led.setMode(2);
}

void checkSerialCommands() {
    if (Serial.available()) { // check if data is available to read
        String command = Serial.readStringUntil('\n'); // read the incoming data until newline character

        if (command.startsWith("led mode")) {
            String idString = command.substring(8);
            int id = idString.toInt();
            led.setMode(id);
            logger.info("Led mode changed");
        } else if (command == "wifi") {
            logger.info("Wifi setup");
        }
    }
}


void loop() {
  led.update();
  checkSerialCommands();
}
