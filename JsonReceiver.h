#ifndef JSON_RECEIVER_H
#define JSON_RECEIVER_H

#include <ArduinoJson.h>
#include <queue>
#include <string>

class JsonReceiver {
public:
    JsonReceiver(size_t bufferLimit = 5, size_t maxSize = 2048);
    void processMessage(const String& message);
    String getMessagesAsString() const;

private:
    std::queue<std::string> messageBuffer;
    size_t bufferLimit;
    size_t maxSize;

    void addMessage(const String& jsonMessage);
};

#endif