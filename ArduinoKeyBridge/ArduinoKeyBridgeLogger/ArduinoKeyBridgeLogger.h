#ifndef ARDUINO_KEY_BRIDGE_LOGGER_H
#define ARDUINO_KEY_BRIDGE_LOGGER_H

#include <Arduino.h>

// Log levels
enum class LogLevel {
    NONE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    MEM
};

// Memory regions for R4 WiFi
#define MEMORY_REGION_SRAM   0
#define MEMORY_REGION_FLASH  1

class ArduinoKeyBridgeLogger {
public:
    static ArduinoKeyBridgeLogger& getInstance();
    
    void begin(unsigned long baudRate = 115200);
    void setLogLevel(LogLevel level);
    
    // Logging methods with source tracking
    void debug(const char* source, const char* message);
    void debug(const char* source, const String& message);
    void info(const char* source, const char* message);
    void info(const char* source, const String& message);
    void warning(const char* source, const char* message);
    void warning(const char* source, const String& message);
    void error(const char* source, const char* message);
    void error(const char* source, const String& message);
    void mem(const char* source, const char* message);
    void mem(const char* source, const String& message);
    
    // Utility methods
    void hexDump(const char* source, const uint8_t* data, size_t length);
    void timestamp();
    void logMemory(const char* source); // Log memory using findMaxAllocation
    
    // Memory test methods
    size_t findMaxAllocation();
    void runMemoryTest();
    void logAllocationTest(const char* source, size_t maxAlloc);
    
    // Memory percentage method
    float getMemoryPercentage();

private:
    ArduinoKeyBridgeLogger() = default;
    ~ArduinoKeyBridgeLogger() = default;
    ArduinoKeyBridgeLogger(const ArduinoKeyBridgeLogger&) = delete;
    ArduinoKeyBridgeLogger& operator=(const ArduinoKeyBridgeLogger&) = delete;

    LogLevel currentLevel = LogLevel::INFO;
    bool initialized = false;
    unsigned long startTime = 0;

    void log(LogLevel level, const char* source, const char* message);
    void log(LogLevel level, const char* source, const String& message);
    const char* getLevelString(LogLevel level);
};

// Convenience macros for logging
#define LOG_DEBUG(message) ArduinoKeyBridgeLogger::getInstance().debug(__FILE__, message)
#define LOG_INFO(message) ArduinoKeyBridgeLogger::getInstance().info(__FILE__, message)
#define LOG_WARNING(message) ArduinoKeyBridgeLogger::getInstance().warning(__FILE__, message)
#define LOG_ERROR(message) ArduinoKeyBridgeLogger::getInstance().error(__FILE__, message)
#define LOG_MEM(message) ArduinoKeyBridgeLogger::getInstance().mem(__FILE__, message)

#endif 