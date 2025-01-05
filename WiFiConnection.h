#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFiS3.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <functional>

class WiFiConnection {
public:
    WiFiConnection(const char* ssid, const char* password, uint16_t port = 80);
    void connect();
    void startServer();
    void handleClient(const std::function<void(const String&)>& onMessageReceived);
    void printStatus();

private:
    const char* ssid;
    const char* password;
    uint16_t port;
    WiFiServer server;
    
};

#endif