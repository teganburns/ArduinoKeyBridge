#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <ArduinoJson.h>
#include <queue>
#include <string>

class JsonHandler {
public:

  static JsonHandler& getInstance();  // Singleton accessor

  //JsonHandler(size_t bufferLimit = 5, size_t maxSize = 2048);
  bool processMessage(const String& message);
  String getMessagesAsString() const;


private:
  std::queue<std::string> messageBuffer;
  size_t bufferLimit;
  size_t maxSize;

  void addMessage(const String& jsonMessage);

  JsonHandler() = default;   // Private constructor
  ~JsonHandler() = default;  // Private destructor

  // Delete copy constructor and assignment operator
  JsonHandler(const JsonHandler&) = delete;
  JsonHandler& operator=(const JsonHandler&) = delete;
};

#endif