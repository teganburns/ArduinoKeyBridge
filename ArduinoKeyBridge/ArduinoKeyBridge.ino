// Required libraries and headers
#include <Arduino.h>
#include "WiFiConnection.h"
#include "ArduinoKeyBridgeNeoPixel.h"
#include "CustomKeyboardParser.h"

// Wi-Fi credentials for network connection
const char* ssid = "BetaRouter";
const char* password = "EDRYquZeM);#2}t";

// USB Host Controller and HID Keyboard interface
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

// Keyboard and Parser instances for handling input
CustomKeyboardParser Parser(MinimalKeyboard::getInstance());


void setup() {
    // Initialize logger first
    ArduinoKeyBridgeLogger::getInstance().begin(115200);
    ArduinoKeyBridgeLogger::getInstance().setLogLevel(LogLevel::NONE); // IDK IF THIS EVEN WORKS 
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Starting ArduinoKeyBridge...");
    
    // Run memory test OPTIONAL
    // ArduinoKeyBridgeLogger::getInstance().runMemoryTest();

    // Run jsonDocumentTest() OPTIONAL
    //ArduinoKeyBridgeLogger::getInstance().jsonDocumentTest();


    // Initialize other components
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Initializing components...");
    
    // Initialize NeoPixel
    ArduinoKeyBridgeNeoPixel::getInstance().begin(6, 8);
    ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(5);
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.0f);  // Start with all red

    // Setup 25% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.25f);  // 25% green
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "25% complete");

    // Initialize WiFi connection and start web server
    WiFiConnection::getInstance().connect(ssid, password);
    WiFiConnection::getInstance().startServer();

    // Log memory after WiFi connection
    ArduinoKeyBridgeLogger::getInstance().logMemory("Setup");

    // Setup 50% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.50f);  // 50% green
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "50% complete");

    // Initialize USB Host Shield
    if (Usb.Init() == -1) {
        ArduinoKeyBridgeLogger::getInstance().error("Setup", "USB initialization failed");
        ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
        while (1); // Halt if USB init fails
    }
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "USB initialized");

    // Setup 75% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.75f);  // 75% green
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "75% complete");

    // Configure keyboard parser and initialize keyboard
    HidKeyboard.SetReportParser(0, &Parser);
    MinimalKeyboard::getInstance().begin();
    
    // Setup 100% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(1.0f);  // All blue

    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Setup completed successfully");
    ArduinoKeyBridgeNeoPixel::getInstance().setStatusIdle();    // Show we're ready


    // Mem after setup
    ArduinoKeyBridgeLogger::getInstance().logMemory("Setup");
}



static unsigned long lastLogTime = 0;
void loop() {

    // Rolls white
    ArduinoKeyBridgeNeoPixel::getInstance().rollColor(NeoPixelColors::WHITE.r << 16 | NeoPixelColors::WHITE.g << 8 | NeoPixelColors::WHITE.b, 0);
    
    // Log Memory every 10 seconds
    if (millis() - lastLogTime >= 10000) {
        lastLogTime = millis();
        ArduinoKeyBridgeLogger::getInstance().logMemory("Loop");
    } 

    // Process USB tasks and handle incoming web clients
    Usb.Task();
    WiFiConnection::getInstance().handleClient();
   
}

/* TODO LIST:
[X] Status LED so I can tell when requests are being sent to server?
[X] Need to make cap touch for arduino reset.
[] capture and send_request will be on dedicated keys . F13 and F14 respectively.
[] F15 will dump the answer out. No typing, not stealth.
[X] Local reset or simply cut power button to reset arduino in the event of overflow, hanging, etc.
*/
