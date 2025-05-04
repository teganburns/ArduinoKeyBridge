#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <queue>
#include <string>
#include "ArduinoKeyBridgeLogger.h"

class JsonHandler {
public:
  static JsonHandler& getInstance();  // Singleton accessor
  bool processMessage(const String& message);
  String getMessagesAsString() const;

private:
  std::queue<std::string> messageBuffer;
  size_t bufferLimit;
  size_t maxSize;

  void addMessage(const String& jsonMessage);

  JsonHandler() = default;   // Private constructor
  ~JsonHandler() = default;  // Private destructor
  JsonHandler(const JsonHandler&) = delete;  // Delete copy constructor
  JsonHandler& operator=(const JsonHandler&) = delete;  // Delete assignment operator
};

#endif