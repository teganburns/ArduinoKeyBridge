#include "ArduinoKeyBridgeLogger.h"

ArduinoKeyBridgeLogger& ArduinoKeyBridgeLogger::getInstance() {
    static ArduinoKeyBridgeLogger instance;
    return instance;
}

void ArduinoKeyBridgeLogger::begin(unsigned long baudRate) {
    if (!initialized) {
        SerialUSB.begin(baudRate);
        while (!SerialUSB); // Wait for Serial connection
        initialized = true;
        startTime = millis();
        info("Logger", "ArduinoKeyBridgeLogger initialized");
    }
}

void ArduinoKeyBridgeLogger::setLogLevel(LogLevel level) {
    currentLevel = level;
    String levelMsg = "Log level set to: ";
    levelMsg += getLevelString(level);
    info("Logger", levelMsg);
}

void ArduinoKeyBridgeLogger::debug(const char* source, const char* message) {
    log(LogLevel::DEBUG, source, message);
}

void ArduinoKeyBridgeLogger::debug(const char* source, const String& message) {
    log(LogLevel::DEBUG, source, message);
}

void ArduinoKeyBridgeLogger::info(const char* source, const char* message) {
    log(LogLevel::INFO, source, message);
}

void ArduinoKeyBridgeLogger::info(const char* source, const String& message) {
    log(LogLevel::INFO, source, message);
}

void ArduinoKeyBridgeLogger::warning(const char* source, const char* message) {
    log(LogLevel::WARNING, source, message);
}

void ArduinoKeyBridgeLogger::warning(const char* source, const String& message) {
    log(LogLevel::WARNING, source, message);
}

void ArduinoKeyBridgeLogger::error(const char* source, const char* message) {
    log(LogLevel::ERROR, source, message);
}

void ArduinoKeyBridgeLogger::error(const char* source, const String& message) {
    log(LogLevel::ERROR, source, message);
}

void ArduinoKeyBridgeLogger::log(LogLevel level, const char* source, const char* message) {
    if (level >= currentLevel && initialized) {
        timestamp();
        SerialUSB.print(getLevelString(level));
        SerialUSB.print(" [");
        SerialUSB.print(source);
        SerialUSB.print("] ");
        SerialUSB.println(message);
    }
}

void ArduinoKeyBridgeLogger::log(LogLevel level, const char* source, const String& message) {
    log(level, source, message.c_str());
}

void ArduinoKeyBridgeLogger::hexDump(const char* source, const uint8_t* data, size_t length) {
    if (!initialized || currentLevel > LogLevel::DEBUG) return;
    
    timestamp();
    SerialUSB.print("DEBUG [");
    SerialUSB.print(source);
    SerialUSB.println("] Hex Dump:");
    
    for (size_t i = 0; i < length; i++) {
        if (data[i] < 0x10) SerialUSB.print("0");
        SerialUSB.print(data[i], HEX);
        SerialUSB.print(" ");
        
        if ((i + 1) % 16 == 0) {
            SerialUSB.println();
        }
    }
    SerialUSB.println();
}

void ArduinoKeyBridgeLogger::timestamp() {
    unsigned long currentTime = millis() - startTime;
    unsigned long seconds = currentTime / 1000;
    unsigned long milliseconds = currentTime % 1000;
    
    SerialUSB.print("[");
    SerialUSB.print(seconds);
    SerialUSB.print(".");
    if (milliseconds < 100) SerialUSB.print("0");
    if (milliseconds < 10) SerialUSB.print("0");
    SerialUSB.print(milliseconds);
    SerialUSB.print("] ");
}

const char* ArduinoKeyBridgeLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        default:               return "UNKNOWN";
    }
} 