#include "WiFiConnection.h"
#include "JsonHandler.h"

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


void setup() {
    // Initialize Serial
    SerialUSB.begin(115200);
    while (!SerialUSB);

    // Connect to Wi-Fi
    WiFiConnection::getInstance().connect(ssid, password);
    WiFiConnection::getInstance().startServer();


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
    WiFiConnection::getInstance().handleClient();

}
