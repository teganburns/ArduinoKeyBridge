#include "JsonHandler.h"
#include <Arduino.h>

JsonHandler& JsonHandler::getInstance() {
  static JsonHandler instance;
  return instance;
}

bool JsonHandler::processMessage(const String& message) {
  SerialUSB.print("Message: ");
  SerialUSB.println(message);
  
  JsonDocument jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, message);

  if (error) {
    SerialUSB.println("Invalid JSON received.");
    SerialUSB.print("Error Message: ");
    SerialUSB.println(error.c_str());
    SerialUSB.print("Hex data: ");
    for (size_t i = 0; i < message.length(); i++) {
      char c = message[i];
      if (c < 16) SerialUSB.print("0");
      SerialUSB.print(c, HEX);
      SerialUSB.print(" ");
    }
    SerialUSB.println();
    return false;
  }

  SerialUSB.println("Valid JSON processed:");
  serializeJsonPretty(jsonDoc, SerialUSB);
  SerialUSB.println();

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
