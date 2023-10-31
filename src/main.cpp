#include <Arduino.h>
#include <Log.h>

const int LED_PIN = 2; // change this to the pin connected to your LED

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200); // initialize serial communication with baud rate of 9600
  logger.info("ESP32 started!");
  logger.error("An error occurred.");
  logger.debug("Debug message.");
  logger.warning("This is a warning.");
}


void loop() {
  for (int brightness = 0; brightness <= 255; brightness++) {
    analogWrite(LED_PIN, brightness);
    delay(10);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    analogWrite(LED_PIN, brightness);
    delay(10);
  }
}
