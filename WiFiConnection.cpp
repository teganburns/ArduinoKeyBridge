#include "WiFiConnection.h"
#include <Arduino.h>

WiFiConnection& WiFiConnection::getInstance() {
  static WiFiConnection instance;
  return instance;
}

void WiFiConnection::connect(const char* ssid_, const char* password_, uint16_t port_) {
  ssid = ssid_;
  password = password_;
  port = port_;

  SerialUSB.println("Starting Wi-Fi connection...");
  if (WiFi.status() == WL_NO_MODULE) {
    SerialUSB.println("Wi-Fi module not found. Stopping execution.");
    while (true);
  }

  SerialUSB.print("Attempting to connect to SSID: ");
  SerialUSB.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    SerialUSB.print(".");
  }

  SerialUSB.println("\nConnected to Wi-Fi!");
  printStatus();
}

void WiFiConnection::startServer() {
  server.begin();
  SerialUSB.println("Wi-Fi server started.");
}

void WiFiConnection::handleClient() {
  WiFiClient client = server.available();
  if (!client) return;

  String requestLine = client.readStringUntil('\n');

  SerialUSB.println("New client connected.");
  SerialUSB.print("Request Line: ");
  SerialUSB.println(requestLine);

  if (requestLine.length() == 0) {
    SerialUSB.println("Empty request line. Closing connection.");
    client.stop();
    return;
  }

  String method = requestLine.substring(0, requestLine.indexOf(' '));
  String path = requestLine.substring(requestLine.indexOf(' ') + 1, requestLine.lastIndexOf(' '));
  path.trim();

  SerialUSB.print("Request Method: ");
  SerialUSB.println(method);
  SerialUSB.print("Request Path: ");
  SerialUSB.println(path);

  if (method == "GET") {
    handleGetRequest(client);
  } else if (method == "POST") {
    handlePostRequest(client);
  } else {
    respondWithError(client, 405, "Method Not Allowed");
  }

  client.stop();
  SerialUSB.println("Client disconnected.");
}

void WiFiConnection::handleGetRequest(WiFiClient& client) {
  SerialUSB.println("Handling GET request...");
  respondWithJson(client, 200, exampleResponse);
}

void WiFiConnection::handlePostRequest(WiFiClient& client) {
  SerialUSB.println("Handling POST request...");

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
      Serial.println("Extracted Body:");
      Serial.println(body);
    } else {
      Serial.println("No body found in the response.");
    }
  } else {
    Serial.println("Client not connected.");
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
  SerialUSB.print("SSID: ");
  SerialUSB.println(WiFi.SSID());
  SerialUSB.print("IP Address: ");
  SerialUSB.println(WiFi.localIP());
  SerialUSB.print("RSSI: ");
  SerialUSB.println(WiFi.RSSI());
}

JsonDocument WiFiConnection::postRequest(const char* serverAddress, int serverPort, const char* resourcePath, const JsonDocument& requestDoc) {
  WiFiClient client;
  JsonDocument responseDoc;

  SerialUSB.print("Connecting to server: ");
  SerialUSB.println(serverAddress);
  if (!client.connect(serverAddress, serverPort)) {
    SerialUSB.println("Connection failed!");
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
    if (millis() - timeout > 5000) {
      SerialUSB.println("Server response timeout.");
      client.stop();
      return responseDoc;
    }
  }

  String response = "";
  while (client.available()) {
    response += client.readString();
  }
  client.stop();

  SerialUSB.println("Response:");
  SerialUSB.println(response);

  int bodyStartIndex = response.indexOf("\r\n\r\n") + 4;
  String responseBody = response.substring(bodyStartIndex);

  DeserializationError error = deserializeJson(responseDoc, responseBody);
  if (error) {
    SerialUSB.print("JSON Parsing failed: ");
    SerialUSB.println(error.c_str());
    responseDoc.clear();
  }

  return responseDoc;
}
