#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

// Define debugging flag
#ifndef WIFI_CONNECTION_DEBUG
#define WIFI_CONNECTION_DEBUG true
#endif

#include <WiFiS3.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <functional>

#include "JsonHandler.h"

class WiFiConnection {
public:

  static WiFiConnection& getInstance();  // Singleton accessor

  void connect(const char* ssid_, const char* password_, uint16_t port_ = 80);
  void startServer();
  void handleClient();
  void printStatus();
  JsonDocument postRequest(const char* serverAddress, int serverPort, const char* resourcePath, const JsonDocument& requestDoc);


private:
  const char* ssid;
  const char* password;
  uint16_t port;
  WiFiServer server;

  const String exampleResponse = R"({
        "message": "Only POST requests are supported.",
        "format": {
            "command": "example_command",
            "data": "optional_payload",
            "time": 1701670456
        }
    })";

  void handleGetRequest(WiFiClient& client);
  void handlePostRequest(WiFiClient& client);
  void respondWithError(WiFiClient& client, int statusCode, const String& error);
  void respondWithJson(WiFiClient& client, int statusCode, const String& message);


  WiFiConnection() = default;   // Private constructor
  ~WiFiConnection() = default;  // Private destructor

  // Delete copy constructor and assignment operator
  WiFiConnection(const WiFiConnection&) = delete;
  WiFiConnection& operator=(const WiFiConnection&) = delete;
};

#endif