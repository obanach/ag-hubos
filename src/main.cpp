#include <Arduino.h>

const int LED_PIN = 2; // change this to the pin connected to your LED

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600); // initialize serial communication with baud rate of 9600
  Serial.println("UART Display Name"); // print the desired name
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
