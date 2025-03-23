#include "JsonHandler.h"
#include <Arduino.h>
#include "ArduinoKeyBridgeLogger.h"

JsonHandler& JsonHandler::getInstance() {
  static JsonHandler instance;
  return instance;
}

bool JsonHandler::processMessage(const String& message) {
  ArduinoKeyBridgeLogger::getInstance().debug("JsonHandler", "Processing message: " + message);
  
  JsonDocument jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, message);

  if (error) {
    ArduinoKeyBridgeLogger::getInstance().error("JsonHandler", String("Invalid JSON: ") + error.c_str());
    return false;
  }

  ArduinoKeyBridgeLogger::getInstance().info("JsonHandler", "Valid JSON processed successfully");
  return true;
}

String JsonHandler::getMessagesAsString() const {
  String combinedMessages;
  std::queue<std::string> tempBuffer = messageBuffer;

  while (!tempBuffer.empty()) {
    combinedMessages += tempBuffer.front().c_str();
    combinedMessages += "\n";
    tempBuffer.pop();
  }

  return combinedMessages;
}

void JsonHandler::addMessage(const String& jsonMessage) {
  SerialUSB.print("Received: ");
  SerialUSB.println(jsonMessage);

  if (messageBuffer.size() >= bufferLimit) {
    messageBuffer.pop();
  }
  messageBuffer.push(jsonMessage.c_str());

  SerialUSB.print("Current message buffer: ");
  std::queue<std::string> tempBuffer = messageBuffer;
  while (!tempBuffer.empty()) {
    SerialUSB.println(tempBuffer.front().c_str());
    tempBuffer.pop();
  }
}
