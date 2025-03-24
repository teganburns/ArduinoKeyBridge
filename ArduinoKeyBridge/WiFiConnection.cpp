#include "WiFiConnection.h"
#include <Arduino.h>
#include "ArduinoKeyBridgeLogger.h"
#include "ArduinoKeyBridgeNeoPixel.h"

WiFiConnection& WiFiConnection::getInstance() {
  static WiFiConnection instance;
  return instance;
}

////////////////////////
// SHARED OPERATIONS //
////////////////////////



////////////////////////
// CLIENT OPERATIONS //
////////////////////////

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






void WiFiConnection::printStatus() {
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("SSID: ") + WiFi.SSID());
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("IP Address: ") + WiFi.localIP().toString());
  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("RSSI: ") + WiFi.RSSI());
}

JsonDocument WiFiConnection::postRequest(const char* serverAddress, int serverPort, const char* resourcePath, const JsonDocument& requestDoc) {

  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "--------------START------------------");
  // variables
  WiFiClient client;
  JsonDocument responseDoc;
  String postData;

  unsigned long startTime = millis();
  const size_t bufferSize = 1024;
  char buffer[bufferSize];
  size_t position = 0;

  serializeJson(requestDoc, postData);
  String request = String("POST ") + resourcePath + " HTTP/1.1\r\n" +
                  "Host: " + serverAddress + "\r\n" +
                  "Content-Type: application/json\r\n" +
                  "Content-Length: " + postData.length() + "\r\n" +
                  "Connection: close\r\n\r\n" +
                  postData;



  ArduinoKeyBridgeLogger::getInstance().info("WiFi", String("Connecting to server: ") + serverAddress);
  ArduinoKeyBridgeNeoPixel::getInstance().setStatusBusy();

  if (!client.connect(serverAddress, serverPort)) {
    ArduinoKeyBridgeLogger::getInstance().error("WiFi", "Failed to connect to server!");
    ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
    return responseDoc;
  } else {
    ArduinoKeyBridgeLogger::getInstance().info("WiFi", "Connected to server!");
  }

  client.println(request);


  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "--------------------------------");
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "Starting to read response...");

  // Wait for initial data with timeout
  unsigned long dataTimeout = millis();
  while (client.available() == 0) {
    if (millis() - dataTimeout > 5000) { // 5 second timeout
      ArduinoKeyBridgeLogger::getInstance().error("WiFi", "No initial data received after 5 seconds");
      client.stop();
      return responseDoc;
    }
    delay(100);
  }

  // Now we have data, keep reading until we're done
  bool foundEndOfBody = false;
  unsigned long lastDataTime = millis();
  bool headersFound = false;
  size_t contentLength = 0;

  while (client.connected() || client.available() > 0) {
    if (client.available()) {
      lastDataTime = millis(); // Reset timeout when we get data
      size_t bytesRead = client.read((uint8_t*)&buffer[position], bufferSize - position);
      position += bytesRead;

      // Look for headers if we haven't found them yet
      if (!headersFound && position >= 4) {
        for (size_t i = 0; i < position - 3; i++) {
          if (buffer[i] == '\r' && buffer[i+1] == '\n' && 
              buffer[i+2] == '\r' && buffer[i+3] == '\n') {
            headersFound = true;
            // Extract content length if present
            String headers = String(buffer, i);
            int clIndex = headers.indexOf("Content-Length: ");
            if (clIndex != -1) {
              contentLength = headers.substring(clIndex + 16).toInt();
            }
            // Move remaining data to start of buffer
            memmove(buffer, buffer + i + 4, position - (i + 4));
            position -= (i + 4);
            break;
          }
        }
      }

      // Check if we've reached the end of the body
      if (headersFound) {
        if (contentLength > 0 && position >= contentLength) {
          foundEndOfBody = true;
          break;
        }
      } else {
        // If no headers found, look for double newline in raw data
        for (size_t i = 0; i < position - 3; i++) {
          if (buffer[i] == '\r' && buffer[i+1] == '\n' && 
              buffer[i+2] == '\r' && buffer[i+3] == '\n') {
            foundEndOfBody = true;
            break;
          }
        }
      }
      
      // If buffer is full, log it and reset
      if (position >= bufferSize - 1) {
        buffer[position] = '\0';
        ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "Buffer chunk:");
        ArduinoKeyBridgeLogger::getInstance().debug("WiFi", buffer);
        position = 0;
      }
    } else {
      // Check if we've waited too long for more data
      if (millis() - lastDataTime > 5000) {
        ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "No new data for 5 seconds, assuming response is complete");
        break;
      }
      delay(50);
    }
  }
  
  // Log any remaining data
  if (position > 0) {
    buffer[position] = '\0';
    ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "Final buffer chunk:");
    ArduinoKeyBridgeLogger::getInstance().debug("WiFi", buffer);
  }

  unsigned long endTime = millis();
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", String("Response time: ") + String(endTime - startTime) + "ms");
  ArduinoKeyBridgeLogger::getInstance().debug("WiFi", "-----------END OF RESPONSE-----------");

  delay(1000); // long delay so I can read logs

  client.stop();

  return responseDoc;
}



////////////////////////
// SERVER OPERATIONS //
////////////////////////

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