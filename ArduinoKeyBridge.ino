// Required libraries and headers
#include "WiFiConnection.h"
#include "JsonHandler.h"
#include "CustomKeyboardParser.h"

// Wi-Fi credentials for network connection
const char* ssid = "BetaRouter";
const char* password = "EDRYquZeM);#2}t";

// USB Host Controller and HID Keyboard interface
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

// Keyboard and Parser instances for handling input
MinimalKeyboard MyKB;
CustomKeyboardParser Parser(MyKB);

void setup() {
    // Initialize Serial communication at 115200 baud
    SerialUSB.begin(115200);
    while (!SerialUSB); // Wait for Serial connection

    // Initialize WiFi connection and start web server
    WiFiConnection::getInstance().connect(ssid, password);
    WiFiConnection::getInstance().startServer();

    // Initialize USB Host Shield
    if (Usb.Init() == -1) {
        SerialUSB.println("USB initialization failed");
        while (1); // Halt if USB init fails
    }
    SerialUSB.println("USB initialized");

    // Configure keyboard parser and initialize keyboard
    HidKeyboard.SetReportParser(0, &Parser);
    MyKB.begin();

    SerialUSB.println("Setup...OK");
}

void loop() {
    // Process USB tasks and handle incoming web clients
    Usb.Task();
    WiFiConnection::getInstance().handleClient();
}
