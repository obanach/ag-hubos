#ifndef LedManager_h
#define LedManager_h

#include <Arduino.h>

class LedManager {
public:
  LedManager(int pin);
  void setMode(int mode);
  void update();

private:
  int ledPin;
  int currentMode;
  unsigned long previousMillis;
  void turnOn();
  void turnOff();
  void fastBlink();
  void normalBlink();
  void slowBlink();
  void pulse();
};

#endif
