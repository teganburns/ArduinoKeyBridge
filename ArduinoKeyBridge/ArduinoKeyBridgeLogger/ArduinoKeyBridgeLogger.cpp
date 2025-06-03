#include "ArduinoKeyBridgeLogger.h"
#include <Arduino.h>
#include <ArduinoJson.h>

ArduinoKeyBridgeLogger& ArduinoKeyBridgeLogger::getInstance() {
    static ArduinoKeyBridgeLogger instance;
    return instance;
}

void ArduinoKeyBridgeLogger::begin(unsigned long baudRate) {
    if (!initialized) {
        SerialUSB.begin(baudRate);
        // No blocking wait for SerialUSB
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
    if (!initialized || !SerialUSB) return;
    log(LogLevel::DEBUG, source, message);
}

void ArduinoKeyBridgeLogger::debug(const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::DEBUG, source, message);
}

void ArduinoKeyBridgeLogger::info(const char* source, const char* message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::INFO, source, message);
}

void ArduinoKeyBridgeLogger::info(const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::INFO, source, message);
}

void ArduinoKeyBridgeLogger::warning(const char* source, const char* message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::WARNING, source, message);
}

void ArduinoKeyBridgeLogger::warning(const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::WARNING, source, message);
}

void ArduinoKeyBridgeLogger::error(const char* source, const char* message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::ERROR, source, message);
}

void ArduinoKeyBridgeLogger::error(const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::ERROR, source, message);
}

void ArduinoKeyBridgeLogger::mem(const char* source, const char* message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::MEM, source, message);
    logMemory(source);
}

void ArduinoKeyBridgeLogger::mem(const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(LogLevel::MEM, source, message);
    logMemory(source);
}

void ArduinoKeyBridgeLogger::log(LogLevel level, const char* source, const char* message) {
    if (!initialized || !SerialUSB) return;
    if (level >= currentLevel) {
        timestamp();
        SerialUSB.print(getLevelString(level));
        SerialUSB.print(" [");
        SerialUSB.print(source);
        SerialUSB.print("] ");
        SerialUSB.println(message);
    }
}

void ArduinoKeyBridgeLogger::log(LogLevel level, const char* source, const String& message) {
    if (!initialized || !SerialUSB) return;
    log(level, source, message.c_str());
}

void ArduinoKeyBridgeLogger::hexDump(const char* source, const uint8_t* data, size_t length) {
    if (!initialized || !SerialUSB || currentLevel > LogLevel::DEBUG) return;
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
    if (!initialized || !SerialUSB) return;
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
        case LogLevel::MEM:     return "MEM";
        default:               return "UNKNOWN";
    }
}

void ArduinoKeyBridgeLogger::logMemory(const char* source) {
    // Memory specifications for Arduino UNO R4 WiFi (RA4M1)
    const uint32_t totalSRAM = 32 * 1024;    // 32 KB SRAM
    
    // Get maximum allocatable block
    size_t maxAlloc = findMaxAllocation();
    
    // Calculate memory usage
    size_t used_memory = totalSRAM - maxAlloc;
    float percentUsed = ((float)used_memory / totalSRAM) * 100;
    
    // Log memory stats
    String memoryStats = String("Memory Usage: ") + 
                         String(percentUsed, 1) + "% (" + 
                         String(used_memory) + "/" + String(totalSRAM) + " bytes), " +
                         "Max Alloc: " + String(maxAlloc) + " bytes";
    log(LogLevel::MEM, source, memoryStats);
}

// Memory test methods implementation, returns max allocatable block
size_t ArduinoKeyBridgeLogger::findMaxAllocation() {
    size_t left = 1;
    size_t right = 32768;  // 32KB max for UNO R4 WiFi
    size_t maxSize = 0;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        void* ptr = malloc(mid);
        
        if (ptr != nullptr) {
            free(ptr);
            maxSize = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return maxSize;
}

// Memory test methods implementation, not commonly used
void ArduinoKeyBridgeLogger::logAllocationTest(const char* source, size_t maxAlloc) {
    String message = String("Max allocatable block: ") + 
                     String(maxAlloc) + " bytes (" + 
                     String(maxAlloc / 1024.0, 1) + " KB)";
    log(LogLevel::MEM, source, message);
}

// Memory test methods implementation, not commonly used
void ArduinoKeyBridgeLogger::runMemoryTest() {
    const char* source = "MemoryTest";
    
    // 1. Initial state
    info(source, "Starting memory test...");
    size_t initialMax = findMaxAllocation();
    logAllocationTest(source, initialMax);
    
    // 2. Allocate a 4KB block
    info(source, "Allocating 4KB block...");
    void* block = malloc(4096);
    if (block == nullptr) {
        error(source, "Failed to allocate 4KB block!");
        return;
    }
    
    // 3. Check max allocation while holding block
    size_t maxWithBlock = findMaxAllocation();
    logAllocationTest(source, maxWithBlock);
    
    // 4. Free the block
    info(source, "Freeing 4KB block...");
    free(block);
    
    // 5. Check max allocation after freeing
    size_t maxAfterFree = findMaxAllocation();
    logAllocationTest(source, maxAfterFree);
    
    // 6. Compare results
    if (maxAfterFree >= initialMax * 0.95) {  // Allow 5% margin
        info(source, "Memory properly reclaimed");
    } else {
        warning(source, "Possible memory leak detected");
    }
}

// Memory percentage method, logMemory() is more comprehensive
float ArduinoKeyBridgeLogger::getMemoryPercentage() {
    // Memory specifications for Arduino UNO R4 WiFi (RA4M1)
    const uint32_t totalSRAM = 32 * 1024;    // 32 KB SRAM
    
    // Get max allocatable block
    int maxAlloc = findMaxAllocation();
    
    // Calculate memory usage percentage
    int used_memory = totalSRAM - maxAlloc;
    return ((float)used_memory / totalSRAM) * 100;
}

void ArduinoKeyBridgeLogger::jsonDocumentTest() {
    const char* source = "JsonTest";
    
    // Initialize JsonDocument for testing
    {
        JsonDocument* doc = new JsonDocument();
        mem(source, "Before JsonDocument creation");
        logMemory(source);
        
        (*doc)["test"] = "test";
        mem(source, "After setting values");
        logMemory(source);
        
        (*doc)["sensors"] = "test";
        mem(source, "After setting sensors");
        logMemory(source);
        
        (*doc)["network"] = "test";
        mem(source, "After setting network values");
        logMemory(source);
        
        // Force document to be used
        String test = (*doc)["test"].as<String>();
        mem(source, "After accessing value");
        logMemory(source);
        
        // Try to reclaim memory
        doc->clear();
        mem(source, "After clearing document");
        logMemory(source);
        
        delete doc;
        mem(source, "After deleting document");
        logMemory(source);
    }
    mem(source, "After scope end");
    logMemory(source);
}