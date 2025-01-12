#include "WiFiConnection.h"
#include <Arduino.h>

WiFiConnection::WiFiConnection(const char* ssid, const char* password, uint16_t port)
  : ssid(ssid), password(password), port(port), server(port) {}

void WiFiConnection::connect() {
  SerialUSB.println("Starting Wi-Fi connection...");
  if (WiFi.status() == WL_NO_MODULE) {
    SerialUSB.println("Wi-Fi module not found. Stopping execution.");
    while (true)
      ;
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

  // Check if the request line is empty (timeout or bad request)
  if (requestLine.length() == 0) {
    SerialUSB.println("Empty request line. Closing connection.");
    client.stop();
    return;
  }

  // Parse HTTP method and path
  String method = requestLine.substring(0, requestLine.indexOf(' '));
  String path = requestLine.substring(requestLine.indexOf(' ') + 1, requestLine.lastIndexOf(' '));
  path.trim();

  SerialUSB.print("Request Method: ");
  SerialUSB.println(method);
  SerialUSB.print("Request Path: ");
  SerialUSB.println(path);

  // Handle GET or POST methods
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

  // Read headers
  /* Dont need this for now but could be useful later
  while (client.available()) {
    String headerLine = client.readStringUntil('\n');
    if (headerLine == "\r") {
      SerialUSB.println("End of headers.");
      break;  // End of headers
    }
    SerialUSB.print("Header: ");
    SerialUSB.println(headerLine);
  }
  */

  String body;

  // Assume client is connected and has data
  if (client) {
    String rawData = "";
    while (client.available()) {
      char c = client.read();
      rawData += c;
    }

    // Find the body by locating the double line break
    int bodyIndex = rawData.indexOf("\r\n\r\n");
    if (bodyIndex != -1) {
      body = rawData.substring(bodyIndex + 4);  // Skip the "\r\n\r\n"
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

  if (jsonHandler.processMessage(body)) {
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
