#include "MinimalKeyboard.h"
#include <HID.h>

// Define the HID report descriptor
const uint8_t MinimalKeyboard::HID_REPORT_DESCRIPTOR[] PROGMEM = {
    //  Keyboard
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)

    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)

    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)

    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x73,                    //   LOGICAL_MAXIMUM (115)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)

    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x73,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

MinimalKeyboard& MinimalKeyboard::getInstance() {
    static MinimalKeyboard instance;
    return instance;
}

MinimalKeyboard::MinimalKeyboard() {
    static HIDSubDescriptor node(HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    HID().AppendDescriptor(&node);
}

void MinimalKeyboard::begin() {
    // No USB.begin() needed for USB Host operation
}

void MinimalKeyboard::sendReport(KeyReport* report) {
    HID().SendReport(2, report, sizeof(KeyReport));
}

void MinimalKeyboard::onNewKeyReport(const uint8_t* buf, uint8_t len) {
    if (len < 8) return; // HID report should be at least 8 bytes

    KeyReport report = {0};
    report.modifiers = buf[1];

    int reportIndex = 0;
    for (int i = 2; i < 8 && reportIndex < 6; ++i) {
        uint8_t keyCode = buf[i];
        if (keyCode == 0) continue;
        report.keys[reportIndex++] = keyCode;
    }

    // Store and flag
    currentReport = report;
    hasNewReport = true;

    // Logging (with key map lookup)
    String logMsg = "New KeyReport: Modifiers: 0x" + String(report.modifiers, HEX) + " Keys:";
    for (int i = 0; i < 6; ++i) {
        logMsg += " 0x" + String(report.keys[i], HEX);
        if (report.keys[i] != 0) {
            for (size_t j = 0; j < unifiedKeyMapSize; ++j) {
                KeyInfo k = unifiedKeyMap[j];
                if (k.hexCode == report.keys[i]) {
                    logMsg += " (" + String(k.description) + ")";
                    break;
                }
            }
        }
    }
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", logMsg);
}
