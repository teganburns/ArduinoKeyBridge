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
  WiFiConnection(const char* ssid, const char* password, uint16_t port = 80);
  void connect();
  void startServer();
  void handleClient();
  void printStatus();

private:
  const char* ssid;
  const char* password;
  uint16_t port;
  WiFiServer server;
  JsonHandler jsonHandler;

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
};

#endif