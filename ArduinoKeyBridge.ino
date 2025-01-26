// Required libraries and headers
#include "WiFiConnection.h"
#include "JsonHandler.h"
#include "CustomKeyboardParser.h"
#include "ArduinoKeyBridgeLogger.h"

// Wi-Fi credentials for network connection
const char* ssid = "BetaRouter";
const char* password = "EDRYquZeM);#2}t";

// USB Host Controller and HID Keyboard interface
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

// Keyboard and Parser instances for handling input
CustomKeyboardParser Parser(MinimalKeyboard::getInstance());

void setup() {
    ArduinoKeyBridgeLogger::getInstance().begin(115200);
    ArduinoKeyBridgeLogger::getInstance().setLogLevel(LogLevel::DEBUG);

    // Initialize WiFi connection and start web server
    WiFiConnection::getInstance().connect(ssid, password);
    WiFiConnection::getInstance().startServer();

    // Initialize USB Host Shield
    if (Usb.Init() == -1) {
        ArduinoKeyBridgeLogger::getInstance().error("Setup", "USB initialization failed");
        while (1); // Halt if USB init fails
    }
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "USB initialized");

    // Configure keyboard parser and initialize keyboard
    HidKeyboard.SetReportParser(0, &Parser);
    MinimalKeyboard::getInstance().begin();

    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Setup completed successfully");
}

void loop() {
    // Process USB tasks and handle incoming web clients
    Usb.Task();
    WiFiConnection::getInstance().handleClient();
}
