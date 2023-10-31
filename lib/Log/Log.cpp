#include "Log.h"

Log logger;

void Log::error(const char* message) {
    Serial.print("[ERROR] ");
    Serial.println(message);
}

void Log::info(const char* message) {
    Serial.print("[INFO] ");
    Serial.println(message);
}

void Log::debug(const char* message) {
    Serial.print("[DEBUG] ");
    Serial.println(message);
}

void Log::warning(const char* message) {
    Serial.print("[WARNING] ");
    Serial.println(message);
}
