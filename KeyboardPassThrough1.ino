#include "WiFiConnection.h"
#include "JsonReceiver.h"

#include "CustomKeyboardParser.h"

// Wi-Fi credentials
const char* ssid = "BetaRouter";
const char* password = "EDRYquZeM);#2}t";

// USB Host setup
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

// Global instances
MinimalKeyboard MyKB;
CustomKeyboardParser Parser(MyKB);

// Objects
WiFiConnection wifiConnection(ssid, password);
JsonReceiver jsonReceiver;


void setup() {
    // Initialize Serial
    SerialUSB.begin(115200);
    while (!SerialUSB);

    // Connect to Wi-Fi
    wifiConnection.connect();
    wifiConnection.startServer();

    // Setup USB Passthrough
    if (Usb.Init() == -1) {
        SerialUSB.println("USB initialization failed");
        while (1);
    }
    SerialUSB.println("USB initialized");

    HidKeyboard.SetReportParser(0, &Parser);
    MyKB.begin();

    SerialUSB.println("Setup...OK");
}

void loop() {

    Usb.Task();

    // Handle incoming Wi-Fi clients and pass messages to JsonReceiver
    wifiConnection.handleClient([](const String& message) {
        jsonReceiver.processMessage(message);
    });

}
