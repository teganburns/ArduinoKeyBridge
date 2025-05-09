#include "TCPConnection.h"
#include "ArduinoKeyBridgeLogger.h"

TCPConnection& TCPConnection::getInstance() {
    static TCPConnection instance;
    return instance;
}

TCPConnection::TCPConnection() {}

void TCPConnection::startAP() {
    WiFi.beginAP(AP_SSID, AP_PASSWORD);
    while (WiFi.status() != WL_AP_LISTENING) {
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Starting access point");
        delay(100);
    }
    ArduinoKeyBridgeLogger::getInstance().info("TCPConnection", "Access Point started");
    ArduinoKeyBridgeLogger::getInstance().info("TCPConnection", String("Local IP address: ") + WiFi.localIP().toString());
    server_.begin();
}

void TCPConnection::poll() {
    // First check for new client connection
    if (!client_ || !client_.connected()) {
        client_ = server_.available();
        if (client_) {
            IPAddress clientIP = client_.remoteIP();
            ArduinoKeyBridgeLogger::getInstance().info("TCPConnection", String("New client connected from IP: ") + clientIP.toString() );
            ready_ = true;
        }
    }

    // Read and log any available data
    if (client_ && client_.connected() && client_.available()) {
        uint8_t buf[32];
        size_t bytesRead = client_.read(buf, min(32, client_.available()));
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", String("Read ") + bytesRead + " bytes:");
        ArduinoKeyBridgeLogger::getInstance().hexDump("TCPConnection", buf, bytesRead);
    }

    // Only process 8-byte key report packets
    if (client_ && client_.connected()) {
        while (client_.available() >= 8) {
            uint8_t buf[8];
            size_t bytesRead = client_.read(buf, 8);
            if (bytesRead == 8) {
                ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Received 8-byte KeyReport from client");
                ArduinoKeyBridgeLogger::getInstance().hexDump("TCPConnection", buf, 8);
                KeyReport report = bufferToKeyReport(buf);
                // Now you can use 'report' as needed, e.g., pass to your keyboard handler
            }
        }
        // Do NOT read or process any text/string messages
    }
}

void TCPConnection::sendKeyReport(const KeyReport& report) {
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Sending key report to client (sendKeyReport)");
    if (client_ && client_.connected()) {
        client_.write((const uint8_t*)&report, sizeof(KeyReport));
        client_.flush();
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Sent key report to client (sendKeyReport)");
    }
}

bool TCPConnection::isReady() const {
    return ready_;
}

void TCPConnection::status() {
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "WiFi Status: " + String(WiFiStatus::toString(WiFi.status())));
}

void TCPConnection::clientStatus() {
    if (client_ && client_.connected()) {
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Client connected: " + client_.remoteIP().toString());
    } else {
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "No client connected.");
    }
}

KeyReport TCPConnection::bufferToKeyReport(const uint8_t* buf) {
    KeyReport report;
    report.modifiers = buf[0];
    report.reserved  = buf[1];
    for (int i = 0; i < 6; ++i) {
        report.keys[i] = buf[2 + i];
    }
    return report;
}