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

    if (client_ && client_.connected()) {

        /*
        if (is_charter_mode()) {
            // Type out packet and then revert to false for charter mode 
            ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Charter executing");
            type_charter(client_.readStringUntil('\0').c_str());
            //charter_mode_ = false;
            //change_mode(KeyReport{0x22, 0x00, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}});
            return;
        }
        */

        if (is_charter_mode()) {
            static String buffer = "";
            while (client_.available() > 0) {
                char c = client_.read();
                if (c == '\0') {
                    if (buffer.length() > 0) {
                        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Charter mode EXITING");
                        charterBuffer = buffer;
                        charter_mode_ = false;
                        change_mode(KeyReport{0x22, 0x00, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}});
                        break;
                    } else {
                        // Ignore empty strings, stay in charter mode
                        continue;
                    }
                } else {
                    buffer += c;
                }
            }
            return;
        }

        // if packet is less than 8 bytes but larger than 0, log it
        if (client_.available() > 0 && client_.available() < 8) {
            ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", "Received incomplete key report: " + String(client_.available()) + " bytes");

        }

        // if packet is too large log it 
        if (client_.available() > 8) {
            ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", "Received too large key report: " + String(client_.available()) + " bytes");
        }

        // Only process 8-byte key report packets
        while (client_.available() >= 8) {
            uint8_t buf[8];
            size_t bytesRead = client_.read(buf, 8);
            
            if (bytesRead == 8) {
                ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Received 8-byte KeyReport from client");
                ArduinoKeyBridgeLogger::getInstance().hexDump("TCPConnection", buf, 8);
                
                KeyReport report = bufferToKeyReport(buf);
                if (change_mode(report)) {
                    if (charter_mode_) break;
                    continue;
                }
                MinimalKeyboard::getInstance().sendReport(&report);
            } else {
                ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", String("Received incomplete key report: ") + bytesRead + " bytes");
                // Clear any remaining data to prevent buffer issues
                while (client_.available()) {
                    client_.read();
                }
            }
        }
    }
}

bool TCPConnection::is_command_mode() {
    return command_mode_;
}

bool TCPConnection::is_charter_mode() {
    return charter_mode_;
}

void TCPConnection::set_command_mode(bool mode) {
    command_mode_ = mode;
}

void TCPConnection::set_charter_mode(bool mode) {
    charter_mode_ = mode;
}

bool TCPConnection::change_mode(const KeyReport& report) {
    // Convert keys to uint8_t for hex comparison
    uint8_t modifiers = report.modifiers;
    uint8_t k0 = (uint8_t)report.keys[0];
    uint8_t k1 = (uint8_t)report.keys[1];
    uint8_t k2 = (uint8_t)report.keys[2];
    uint8_t k3 = (uint8_t)report.keys[3];
    uint8_t k4 = (uint8_t)report.keys[4];
    uint8_t k5 = (uint8_t)report.keys[5];

    // Example "switch" on key report patterns
    if (modifiers == 0x22 && k0 == 10 && k1 == 10 && k2 == 10 && k3 == 10 && k4 == 10 && k5 == 10) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::BLUE);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL -10 (e.g., command mode ON)");
        //TCPConnection::getInstance().sendKeyReport(KeyReport{0x22, 0x00, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10}});
        return true;

    } else if (modifiers == 0x22 && k0 == 11 && k1 == 11 && k2 == 11 && k3 == 11 && k4 == 11 && k5 == 11) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::WHITE);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL 11 (e.g., command mode OFF)");
        //TCPConnection::getInstance().sendKeyReport(KeyReport{0x22, 0x00, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}});
        return true;

    } else if (modifiers == 0x22 && k0 == 12 && k1 == 12 && k2 == 12 && k3 == 12 && k4 == 12 && k5 == 12) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::GREEN);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL 12 (another custom command)");
        // good command
        return true;
    } else if (modifiers == 0x22 && k0 == 13 && k1 == 13 && k2 == 13 && k3 == 13 && k4 == 13 && k5 == 13) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::RED);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL 13 (another custom command)");
        // bad command
        return true;
    } else if (modifiers == 0x22 && k0 == 14 && k1 == 14 && k2 == 14 && k3 == 14 && k4 == 14 && k5 == 14) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::YELLOW);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL 14 (another custom command)");
        // bad command
        return true;
    } else if (modifiers == 0x22 && k0 == 2 && k1 == 2 && k2 == 2 && k3 == 2 && k4 == 2 && k5 == 2) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::MAGENTA);
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Special report: ALL 2 (charter mode)");
        charter_mode_ = true;
        // bad command
        return true;
    }
    // ...add more patterns as needed...

    // Default: not a special report
    return false;
}

void TCPConnection::sendKeyReport(const KeyReport& report) {
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Sending key report to client (sendKeyReport)");
    if (client_ && client_.connected()) {
        client_.write((const uint8_t*)&report, sizeof(KeyReport));
        client_.flush();
        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Sent key report to client (sendKeyReport)");
    }
}

void TCPConnection::sendEmptyKeyReport() {
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Sending empty key report to client (sendEmptyKeyReport)");
    if (client_ && client_.connected()) {
        KeyReport emptyKeyReport = {0};
        client_.write((const uint8_t*)&emptyKeyReport, sizeof(KeyReport));
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

void TCPConnection::type_charter(const char* str) {
    ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Typing charter: " + String(str));
    for (size_t i = 0; i < strlen(str); ++i) {
        char c = str[i];
        KeyReport report = {0};
        bool found = false;

        // Search the key map for the character
        for (size_t j = 0; j < unifiedKeyMapSize; ++j) {
            if (unifiedKeyMap[j].asciiValue == c) {
                report.keys[0] = unifiedKeyMap[j].hexCode;
                report.modifiers = unifiedKeyMap[j].shifted ? 0x02 : 0x00; // Set Shift if needed
                found = true;
                break;
            }
        }

        if (found) {
            MinimalKeyboard::getInstance().sendReport(&report); // Key down
            delay(8); // Small delay for key press
            KeyReport release = {0}; // Key up (release)
            MinimalKeyboard::getInstance().sendReport(&release);
            delay(2); // Small delay for key release
        } else {
            // Optionally log or handle unmapped characters
            ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", String("No keycode for char: ") + c);
        }
    }
}

void TCPConnection::toggleCharterMode() {
    bool prev = charter_mode_;
    charter_mode_ = !charter_mode_;
    ArduinoKeyBridgeLogger::getInstance().info("CharterMode", String("Manual charter mode toggled from ") + (prev ? "ON" : "OFF") + " to " + (charter_mode_ ? "ON" : "OFF"));
    ArduinoKeyBridgeNeoPixel::getInstance().setColor(charter_mode_ ? NeoPixelColors::MAGENTA : NeoPixelColors::WHITE);
    ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(charter_mode_ ? 15 : 1);
}

void TCPConnection::handleCharterKeyReport(const KeyReport& report) {
    // Ignore empty key reports (key up events)
    bool isEmpty = (report.modifiers == 0x00);
    for (int i = 0; i < 6; ++i) { if (report.keys[i] != 0x00) { isEmpty = false; break; } }
    if (isEmpty) { return; }

    uint8_t key = report.keys[0];
    switch (key) {
        case 0x6E: // F19
            toggleCharterMode();
            break;
        case 0x6D: // F18
            dumpCharterBuffer();
            break;
        case 0x6C: // F17
            clearCharterBuffer();
            break;
        default:
            typeNextCharFromBuffer();
            break;
    }
}

void TCPConnection::clearCharterBuffer() {
    if (charter_mode_) {
        charterBuffer = "";
        // Optionally update LEDs or log
    }
}

void TCPConnection::dumpCharterBuffer() {
    if (charter_mode_ && charterBuffer.length() > 0) {
        bool shitToDump = true;
        while (charterBuffer.length() > 0) {
            typeNextCharFromBuffer();
        }


        // Optionally update LEDs or log
    }
}

void TCPConnection::typeNextCharFromBuffer() {
    if (charter_mode_ && charterBuffer.length() > 0) {
        char c = charterBuffer[0];
        charterBuffer.remove(0, 1);

        char singleCharStr[2] = {c, '\0'};
        type_charter(singleCharStr);

        ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Typed next char from buffer: " + String(c));
        // Optionally update LEDs if buffer is now empty
        if (charterBuffer.length() == 0) {
            ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::GREEN);
            ArduinoKeyBridgeLogger::getInstance().debug("TCPConnection", "Charter buffer is now empty, setting color to GREEN");
        }
        return;
    } else if (charterBuffer.length() == 0) {
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::RED);
        ArduinoKeyBridgeLogger::getInstance().warning("TCPConnection", "Charter mode on but buffer is empty");
        return;
    }
}