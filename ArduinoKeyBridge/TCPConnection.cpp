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

    // Only process 8-byte key report packets
    if (client_ && client_.connected()) {
        while (client_.available() >= 8) {
            uint8_t buf[8];
            size_t bytesRead = client_.read(buf, 8);
            
            if (bytesRead == 8) {
                ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Received 8-byte KeyReport from client");
                ArduinoKeyBridgeLogger::getInstance().hexDump("TCPConnection", buf, 8);
                KeyReport report = bufferToKeyReport(buf);
                MinimalKeyboard::getInstance().sendReport(&report);
            } else {
                ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", 
                    String("Received incomplete key report: ") + bytesRead + " bytes");
                // Clear any remaining data to prevent buffer issues
                while (client_.available()) {
                    client_.read();
                }
            }
        }
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

// Helper function to convert a buffer from the client to a KeyReport
// TODO: Client will need to start sending valid key reports that can be fed directly the the MinimalKeyboard::SendReport function
KeyReport TCPConnection::bufferToKeyReport(const uint8_t* buf) {
    KeyReport report;
    report.modifiers = buf[0];
    report.reserved = buf[1];
    
    // Log the modifiers
    String modifierStr = "";
    if (report.modifiers & 0x01) modifierStr += "CTRL ";
    if (report.modifiers & 0x02) modifierStr += "SHIFT ";
    if (report.modifiers & 0x04) modifierStr += "ALT ";
    if (report.modifiers & 0x08) modifierStr += "GUI ";
    if (report.modifiers & 0x10) modifierStr += "LEFT_CTRL ";
    if (report.modifiers & 0x20) modifierStr += "LEFT_SHIFT ";
    if (report.modifiers & 0x40) modifierStr += "LEFT_ALT ";
    if (report.modifiers & 0x80) modifierStr += "LEFT_GUI ";
    
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", 
        "Received modifiers: 0x" + String(report.modifiers, HEX) + " (" + modifierStr + ")");

    // Process each key
    for (int i = 0; i < 6; ++i) {
        report.keys[i] = buf[2 + i];
        
        // If there's a key, look it up in the keymap
        if (report.keys[i] != 0) {
            bool found = false;
            for (size_t j = 0; j < unifiedKeyMapSize; j++) {
                if (unifiedKeyMap[j].hexCode == report.keys[i]) {
                    // Check if this is the correct shifted/unshifted version
                    bool isShifted = (report.modifiers & 0x02) != 0;
                    if (unifiedKeyMap[j].shifted == isShifted) {
                        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", 
                            String("Key[") + i + "]: 0x" + String(report.keys[i], HEX) + 
                            " (" + unifiedKeyMap[j].description + ")");
                        found = true;
                        break;
                    }
                }
            }
            
            if (!found) {
                ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", 
                    String("Unknown key code: 0x") + String(report.keys[i], HEX));
            }
        }
    }

    // Log the full report
    String reportStr = "Full KeyReport: [";
    for (int i = 0; i < 6; i++) {
        if (i > 0) reportStr += ", ";
        reportStr += "0x" + String(report.keys[i], HEX);
    }
    reportStr += "]";
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", reportStr);

    return report;
}