#include "WiFiConnection.h"
#include <Arduino.h>

WiFiConnection::WiFiConnection(const char* ssid, const char* password, uint16_t port) : ssid(ssid), password(password), port(port), server(port) {}

void WiFiConnection::connect() {
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

void WiFiConnection::handleClient(const std::function<void(const String&)>& onMessageReceived) {
    WiFiClient client = server.available();
    if (!client) return;

    Serial.println("New client connected.");
    String headers = "", body = "";
    int contentLength = 0;
    bool headersComplete = false;

    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            if (!headersComplete) {
                headers += c;
                if (headers.endsWith("\r\n\r\n")) {
                    if (headers.indexOf("Content-Length:") >= 0) {
                        int startIndex = headers.indexOf("Content-Length:") + 15;
                        int endIndex = headers.indexOf("\r\n", startIndex);
                        contentLength = headers.substring(startIndex, endIndex).toInt();
                    }
                    headersComplete = true;
                }
            } else {
                while (body.length() < contentLength && client.available()) {
                    body += client.read();
                }
                if (body.length() == contentLength) break;
            }
        }
    }

    if (!body.isEmpty()) {
        Serial.println("Message received:");
        Serial.println(body);
        onMessageReceived(body);
    }

    client.stop();
    Serial.println("Client disconnected.");
}

void WiFiConnection::printStatus() {
    SerialUSB.print("SSID: ");
    SerialUSB.println(WiFi.SSID());
    SerialUSB.print("IP Address: ");
    SerialUSB.println(WiFi.localIP());
    SerialUSB.print("RSSI: ");
    SerialUSB.println(WiFi.RSSI());
}