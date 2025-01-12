#include "JsonHandler.h"
#include <Arduino.h>

JsonHandler::JsonHandler(size_t bufferLimit, size_t maxSize)
  : bufferLimit(bufferLimit), maxSize(maxSize) {}

bool JsonHandler::processMessage(const String& message) {

  SerialUSB.print("Message: ");
  SerialUSB.println(message);
  StaticJsonDocument<2048> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, message);

  if (error) {
    SerialUSB.println("Invalid JSON received.");
    SerialUSB.print("Error Message: ");
    SerialUSB.println(error.c_str());
    SerialUSB.print("Hex data: ");
    for (size_t i = 0; i < message.length(); i++) {
      char c = message[i];
      if (c < 16) Serial.print("0");  // Add leading zero for single-digit hex
      Serial.print(c, HEX);
      Serial.print(" ");  // Separate bytes
    }
    Serial.println();  // End line after printing


    return false;
  }

  //addMessage(message); // TODO: Need to fix, requests hang here

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
  // Print all messages in the queue
  while (!messageBuffer.empty()) {
    // Get the front message
    std::string message = messageBuffer.front();

    // Print the message
    Serial.println(message.c_str());
  }
}
