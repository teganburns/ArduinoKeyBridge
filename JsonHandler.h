#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <ArduinoJson.h>
#include <queue>
#include <string>

class JsonHandler {
public:
    JsonHandler(size_t bufferLimit = 5, size_t maxSize = 2048);
    bool processMessage(const String& message);
    String getMessagesAsString() const;

private:
    std::queue<std::string> messageBuffer;
    size_t bufferLimit;
    size_t maxSize;

    void addMessage(const String& jsonMessage);
};

#endif