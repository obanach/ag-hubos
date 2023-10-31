#include "LedManager.h"

LedManager::LedManager(int pin) {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);
  currentMode = 0;
  previousMillis = 0;
}

void LedManager::setMode(int mode) {
  currentMode = mode;
}

void LedManager::update() {
  switch (currentMode) {
    case 0:
      turnOff();
      break;
    case 1:
      turnOn();
      break;
    case 2:
      fastBlink();
      break;
    case 3:
      normalBlink();
      break;
    case 4:
      slowBlink();
      break;
    case 5:
      pulse();
      break;
    default:
      break;
  }
}

void LedManager::turnOn() {
  digitalWrite(ledPin, HIGH);
}

void LedManager::turnOff() {
  digitalWrite(ledPin, LOW);
}

void LedManager::fastBlink() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 100) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}

void LedManager::normalBlink() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}

void LedManager::slowBlink() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}

void LedManager::pulse() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 100) {
    previousMillis = currentMillis;
    int value = analogRead(ledPin);
    analogWrite(ledPin, value < 255 ? value + 1 : 0);
  }
}
