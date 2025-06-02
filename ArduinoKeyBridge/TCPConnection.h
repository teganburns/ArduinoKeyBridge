#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <Arduino.h>
#include <WiFiS3.h>
#include "MinimalKeyboard.h" // For KeyReport
#include "ArduinoKeyBridgeNeoPixel.h"

class TCPConnection {
public:
    static TCPConnection& getInstance();

    // Send a key report to connected clients
    void sendKeyReport(const KeyReport& report);
    void sendEmptyKeyReport();

    // Start the WiFi Access Point
    void startAP();
    void poll();
    bool isReady() const;
    void status();
    void clientStatus();
    bool change_mode(const KeyReport& report);
    void set_command_mode(bool mode);
    bool is_command_mode();
    bool is_charter_mode();
    void set_charter_mode(bool mode);

    // Charter mode/local typing support
    void handleCharterKeyReport(const KeyReport& report);
    void toggleCharterMode();
    void clearCharterBuffer();
    void dumpCharterBuffer();
    void typeNextCharFromBuffer();
    String charterBuffer = "This is a test string";

    void type_charter(const char* str);

    KeyReport bufferToKeyReport(const uint8_t* buf);

private:
    static constexpr uint16_t PORT = 8080;
    static constexpr const char* AP_SSID = "ArduinoKeyBridge";
    static constexpr const char* AP_PASSWORD = "12345678";

    struct WiFiStatus {
        static const char* toString(int status) {
            switch (status) {
                case WL_IDLE_STATUS: return "IDLE";
                case WL_NO_SSID_AVAIL: return "NO_SSID_AVAIL";
                case WL_CONNECT_FAILED: return "CONNECT_FAILED";
                case WL_CONNECTED: return "CONNECTED";
                case WL_DISCONNECTED: return "DISCONNECTED";
                case WL_AP_LISTENING: return "AP_LISTENING";
                case WL_AP_CONNECTED: return "AP_CONNECTED";
                case WL_AP_FAILED: return "AP_FAILED";
                default: return "UNKNOWN";
            }
        }
    };

    WiFiServer server_ = WiFiServer(PORT);
    WiFiClient client_;
    bool ready_ = false;
    bool command_mode_ = false;
    bool charter_mode_ = false;

    // Private constructor for singleton pattern
    TCPConnection();
    ~TCPConnection() = default;
    TCPConnection(const TCPConnection&) = delete;
    TCPConnection& operator=(const TCPConnection&) = delete;
};

#endif // TCP_CONNECTION_H 