// Required libraries and headers
#include <Arduino.h>
#include <memory>
#include "TCPConnection/TCPConnection.h"
#include "ArduinoKeyBridgeNeoPixel/ArduinoKeyBridgeNeoPixel.h"
#include "MinimalKeyboard/MinimalKeyboard.h"

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

bool isCharterMode = false;
String charterBuffer = "";
unsigned long lastCharTime = 0;
const unsigned long charterDelay = 100; // ms

void setup() {
    // Initialize logger first
    ArduinoKeyBridgeLogger::getInstance().begin(115200);
    ArduinoKeyBridgeLogger::getInstance().setLogLevel(LogLevel::DEBUG);
    ArduinoKeyBridgeLogger::getInstance().info("Setup", "Starting ArduinoKeyBridge...");
    
    // Initialize NeoPixel
    ArduinoKeyBridgeNeoPixel::getInstance().begin(6, 8);
    ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(0);
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
    ArduinoKeyBridgeNeoPixel::getInstance().setColor(NeoPixelColors::WHITE);

    // Mem after setup
    ArduinoKeyBridgeLogger::getInstance().logMemory("Setup");
}


void handle_new_key_report() {

    if (keyboard.currentReport.keys[0] == 0x6E) { // F19 keycode
        // Manual charter mode toggle (F19)
        TCPConnection::getInstance().toggleCharterMode();
        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Manual charter mode toggled - Now " + String(TCPConnection::getInstance().is_charter_mode() ? "ON" : "OFF"));
        keyboard.hasNewReport = false;
        return;

    } else if (TCPConnection::getInstance().is_charter_mode()) {
        // Handle charter mode key reports
        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Charter mode is on, handling key report");
        TCPConnection::getInstance().handleCharterKeyReport(keyboard.currentReport);
        keyboard.hasNewReport = false;
        return;

    } else if (keyboard.currentReport.modifiers == 0x22) {
        // Always process command mode toggle reports locally first

        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Command mode detected");
        TCPConnection::getInstance().set_command_mode(!TCPConnection::getInstance().is_command_mode());
        ArduinoKeyBridgeNeoPixel::getInstance().setColor(TCPConnection::getInstance().is_command_mode() ? NeoPixelColors::BLUE : NeoPixelColors::WHITE);
        //TCPConnection::getInstance().sendKeyReport(&keyboard.currentReport); // Optionally notify server
        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Command mode toggled");
        if (TCPConnection::getInstance().is_command_mode()) {
            ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Command mode ON");
            TCPConnection::getInstance().sendKeyReport(KeyReport{0x22, 0x00, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10}});
            ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(15);
        } else {
            ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Command mode OFF");
            TCPConnection::getInstance().sendKeyReport(KeyReport{0x22, 0x00, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}});
            ArduinoKeyBridgeNeoPixel::getInstance().setBrightness(1);
        }
        // The is_keyreport_command_mode function should toggle the mode internally
    } else if (TCPConnection::getInstance().is_command_mode()) {
        // In command mode: send all other key reports to the server
        TCPConnection::getInstance().sendKeyReport(keyboard.currentReport);
        ArduinoKeyBridgeLogger::getInstance().debug("Loop", "Sending key report to TCP connection");
    
    } else {
        // Otherwise, send to the host computer
        keyboard.sendReport(&keyboard.currentReport);
    }
    keyboard.hasNewReport = false;
}


void loop() {
    // Roll the currently active color
    ArduinoKeyBridgeNeoPixel::getInstance().rollColor(0);

    // Process USB tasks
    Usb.Task();

    // Check for TCP connection client/new message
    TCPConnection::getInstance().poll();

    // Check for new key report and send to TCP connection
    if (keyboard.hasNewReport) {
        handle_new_key_report();
    }

    // Call TCPConnection::status() every 10 seconds
    if (millis() - lastStatusTime >= STATUS_INTERVAL) {
        lastStatusTime = millis();
        TCPConnection::getInstance().status();

        // send a key report to the TCP connection
        /*
        KeyReport report = {0};
        report.modifiers = 0x00; // No modifiers
        report.reserved = 0x00;
        report.keys[0] = 0x37; // '.' key
        for (int i = 1; i < 6; ++i) report.keys[i] = 0x00;
        TCPConnection::getInstance().sendKeyReport(report);
        TCPConnection::getInstance().sendEmptyKeyReport();
        */
    }
}


