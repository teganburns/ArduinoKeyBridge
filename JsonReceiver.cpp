#include "JsonReceiver.h"
#include <Arduino.h>

JsonReceiver::JsonReceiver(size_t bufferLimit, size_t maxSize)
    : bufferLimit(bufferLimit), maxSize(maxSize) {}

void JsonReceiver::processMessage(const String& message) {
    StaticJsonDocument<2048> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, message);

    if (error) {
        SerialUSB.println("Invalid JSON received.");
        return;
    }

    addMessage(message);
    SerialUSB.println("Valid JSON processed:");
    serializeJsonPretty(jsonDoc, Serial);
    SerialUSB.println();
}

String JsonReceiver::getMessagesAsString() const {
    String combinedMessages;
    std::queue<std::string> tempBuffer = messageBuffer;

    while (!tempBuffer.empty()) {
        combinedMessages += tempBuffer.front().c_str();
        combinedMessages += "\n";
        tempBuffer.pop();
    }

    return combinedMessages;
}

void JsonReceiver::addMessage(const String& jsonMessage) {
    if (messageBuffer.size() >= bufferLimit) {
        messageBuffer.pop();
    }
    messageBuffer.push(jsonMessage.c_str());
}