#include <Arduino.h>
#include <Log.h>
#include <LedManager.h>

const String VERSION = "0.1";
const int LED_PIN = 2; // change this to the pin connected to your LED

LedManager led(LED_PIN);

void setup() {
  Serial.begin(115200); // initialize serial communication with baud rate of 9600
  String message = "AG-HubOS v" + VERSION + " started";
  logger.info(message.c_str());
  led.setMode(2);
}


void loop() {
  led.update();
}
