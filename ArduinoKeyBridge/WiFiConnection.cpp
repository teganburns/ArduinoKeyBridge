#include "WiFiConnection.h"
#include <Arduino.h>
#include "ArduinoKeyBridgeLogger.h"
#include "ArduinoKeyBridgeNeoPixel.h"

WiFiConnection& WiFiConnection::getInstance() {
  static WiFiConnection instance;
  return instance;
}

void WiFiConnection::connect(const char* ssid_, const char* password_, uint16_t port_) {
  ssid = ssid_;
  password = password_;
  port = port_;

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Starting Wi-Fi connection...");
  if (WiFi.status() == WL_NO_MODULE) {
    ArduinoKeyBridgeLogger::getInstance().error("WiFi", "Wi-Fi module not found. Stopping execution.");
    while (true);
  }

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("Attempting to connect to SSID: ") + ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    ArduinoKeyBridgeLogger::getInstance().debug("WiFi", ".");
  }

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Connected to Wi-Fi!");
  printStatus();

  // Log memory usage
  ArduinoKeyBridgeLogger::getInstance().logMemory("WiFi");
}

void WiFiConnection::startServer() {
  server.begin();
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Wi-Fi server started.");
}

void WiFiConnection::handleClient() {
  WiFiClient client = server.available();
  if (!client) return;

  String requestLine = client.readStringUntil('\n');

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "New client connected.");
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("Client IP: ") + client.remoteIP().toString());
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", String("Request Line: ") + requestLine);

  if (requestLine.length() == 0) {
    ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "Empty request line. Closing connection.");
    client.stop();
    return;
  }

  String method = requestLine.substring(0, requestLine.indexOf(' '));
  String path = requestLine.substring(requestLine.indexOf(' ') + 1, requestLine.lastIndexOf(' '));
  path.trim();

  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", String("Request Method: ") + method);
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", String("Request Path: ") + path);

  if (method == "GET") {
    handleGetRequest(client);
  } else if (method == "POST") {
    handlePostRequest(client);
  } else {
    respondWithError(client, 405, "Method Not Allowed");
  }

  client.stop();
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Client disconnected.");
}

void WiFiConnection::handleGetRequest(WiFiClient& client) {
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Handling GET request...");
  respondWithJson(client, 200, exampleResponse);
}

void WiFiConnection::handlePostRequest(WiFiClient& client) {
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Handling POST request...");

  String body;
  if (client) {
    String rawData = "";
    while (client.available()) {
      char c = client.read();
      rawData += c;
    }

    int bodyIndex = rawData.indexOf("\r\n\r\n");
    if (bodyIndex != -1) {
      body = rawData.substring(bodyIndex + 4);
      ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Extracted Body:");
      ArduinoKeyBridgeLogger::getInstance().debug("WiFi", body);
    } else {
      ArduinoKeyBridgeLogger::getInstance().info("WiFi", "No body found in the response.");
    }
  } else {
    ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Client not connected.");
  }

  if (body.isEmpty()) {
    respondWithError(client, 400, "Empty JSON payload");
    return;
  }

  if (JsonHandler::getInstance().processMessage(body)) {
    respondWithJson(client, 200, R"({"status": "success"})");
  } else {
    respondWithError(client, 400, "Invalid JSON");
  }

  client.stop();
}

void WiFiConnection::respondWithError(WiFiClient& client, int statusCode, const String& error) {
  String message = "{\"error\":\"" + error + "\"}";
  respondWithJson(client, statusCode, message);
}

void WiFiConnection::respondWithJson(WiFiClient& client, int statusCode, const String& message) {
  client.print("HTTP/1.1 ");
  client.print(statusCode);
  client.println(" OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.println(message);
}

void WiFiConnection::printStatus() {
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("SSID: ") + WiFi.SSID());
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("IP Address: ") + WiFi.localIP().toString());
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("RSSI: ") + WiFi.RSSI());
}

JsonDocument WiFiConnection::postRequest(const char* serverAddress, int serverPort, const char* resourcePath, const JsonDocument& requestDoc) {
  WiFiClient client;
  JsonDocument responseDoc;

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("Connecting to server: ") + serverAddress);
  ArduinoKeyBridgeNeoPixel::getInstance().setStatusBusy();

  if (!client.connect(serverAddress, serverPort)) {
    ArduinoKeyBridgeLogger::getInstance().error("WiFi", "Connection failed!");
    ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
    return responseDoc;
  }

  String postData;
  serializeJson(requestDoc, postData);

  client.print("POST ");
  client.print(resourcePath);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(serverAddress);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(postData.length());
  client.println("Connection: close");
  client.println();
  client.println(postData);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 30000) { // 30 seconds
      ArduinoKeyBridgeLogger::getInstance().error("WiFi", "Server response timeout. Currently set to 10 seconds.");
      client.stop();
      ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
      return responseDoc;
    }
  }

  String response = "";
  // start timer
  unsigned long startTime = millis();
  while (client.available()) {
    response += client.readString(); // TODO: this takes too long
  }
  unsigned long endTime = millis();
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", String("Response time: ") + String(endTime - startTime) + "ms");
  client.stop();

  ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Response:");
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", response);

  int bodyStartIndex = response.indexOf("\r\n\r\n") + 4;
  String responseBody = response.substring(bodyStartIndex);

  DeserializationError error = deserializeJson(responseDoc, responseBody);
  if (error) {
    ArduinoKeyBridgeLogger::getInstance().error("WiFi", String("JSON Parsing failed: ") + error.c_str());
    responseDoc.clear();
    ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
  }
  ArduinoKeyBridgeNeoPixel::getInstance().setStatusSuccess();
  delay(1000);
  ArduinoKeyBridgeNeoPixel::getInstance().setStatusIdle();

  // Log memory usage
  ArduinoKeyBridgeLogger::getInstance().logMemory("WiFi");

  return responseDoc;
}
