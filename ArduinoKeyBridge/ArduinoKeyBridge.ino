// Required libraries and headers
#include <Arduino.h>
#include "TCPConnection.h"
#include "ArduinoKeyBridgeNeoPixel.h"
#include "MinimalKeyboard.h"

// USB Host Controller and HID Keyboard interface
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

// Keyboard instance for handling input
MinimalKeyboard& keyboard = MinimalKeyboard::getInstance();
MinimalKeyboardParser parser(keyboard);

// Timer for test key report
static unsigned long lastTestKeyTime = 0;
static constexpr unsigned long TEST_KEY_INTERVAL = 20000; // 20 seconds

static unsigned long lastStatusTime = 0;
static constexpr unsigned long STATUS_INTERVAL = 10000; // 10 seconds

void setup() {
    // Initialize logger first
    ArduinoKeyBridgeLogger::getInstance().begin(115200);
    ArduinoKeyBridgeLogger::getInstance().setLogLevel(LogLevel::DEBUG);
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Starting ArduinoKeyBridge...");
    
    // Initialize NeoPixel
    ArduinoKeyBridgeNeoPixel::getInstance().begin(6, 8);
    ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(5);
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.0f);

    // Setup 25% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.25f);
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "25% complete");

    // Initialize TCP server
    TCPConnection::getInstance().startAP();

    // Setup 50% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.50f);
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "50% complete");

    // Initialize USB Host Shield
    if (Usb.Init() == -1) {
        ArduinoKeyBridgeLogger::getInstance().error("Setup", "USB initialization failed");
        ArduinoKeyBridgeNeoPixel::getInstance().setStatusError();
        while (1); // Halt if USB init fails
    }
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "USB initialized");

    // Register the keyboard parser
    HidKeyboard.SetReportParser(0, &parser);

    // Setup 75% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(0.75f);
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "75% complete");

    // Initialize keyboard
    keyboard.begin();
    
    // Setup 100% complete
    ArduinoKeyBridgeNeoPixel::getInstance().showSetupProgress(1.0f);

    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Setup completed successfully");
    ArduinoKeyBridgeNeoPixel::getInstance().setStatusIdle();

    // Mem after setup
    ArduinoKeyBridgeLogger::getInstance().logMemory("Setup");
}

void loop() {
    // Rolls white for visual indication of loop() frequency
    ArduinoKeyBridgeNeoPixel::getInstance().rollColor(NeoPixelColors::WHITE.r << 16 | NeoPixelColors::WHITE.g << 8 | NeoPixelColors::WHITE.b, 0);
    

    // Process USB tasks
    Usb.Task();

    // Check for TCP connection client/new message
    TCPConnection::getInstance().poll();

    // Check for new key report and send to TCP connection
    if (keyboard.hasNewReport) {
        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Sending key report to TCP connection");
        TCPConnection::getInstance().sendKeyReport(keyboard.currentReport);
        // dont send to keyboard, just log it
        //keyboard.sendReport(&keyboard.currentReport);
        keyboard.hasNewReport = false;
    }


    // Call TCPConnection::status() every 10 seconds
    if (millis() - lastStatusTime >= STATUS_INTERVAL) {
        lastStatusTime = millis();
        TCPConnection::getInstance().status();

        // send a key report to the TCP connection
        KeyReport report = {0};
        report.modifiers = 0x00; // No modifiers
        report.reserved = 0x00;
        report.keys[0] = 0x37; // '.' key
        for (int i = 1; i < 6; ++i) report.keys[i] = 0x00;
        TCPConnection::getInstance().sendKeyReport(report);
        TCPConnection::getInstance().sendEmptyKeyReport();
    }
}


