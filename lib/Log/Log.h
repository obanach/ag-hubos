#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

class Log {
public:
    Log() {}

    void error(const char* message);
    void info(const char* message);
    void debug(const char* message);
    void warning(const char* message);
};

extern Log logger;

#endif
