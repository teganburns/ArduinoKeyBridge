#include "MinimalKeyboard.h"

// Define the HID report descriptor in the .cpp file
/*
const uint8_t MinimalKeyboard::HID_REPORT_DESCRIPTOR[] PROGMEM = {
    // Keyboard descriptor
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    // REPORT_ID (2)
    0x05, 0x07,                    // USAGE_PAGE (Keyboard)
    0x19, 0xE0,                    // USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xE7,                    // USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x25, 0x01,                    // LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    // REPORT_SIZE (1)
    0x95, 0x08,                    // REPORT_COUNT (8)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)
    0x95, 0x01,                    // REPORT_COUNT (1)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x81, 0x03,                    // INPUT (Cnst,Var,Abs)
    0x95, 0x06,                    // REPORT_COUNT (6)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x25, 0x73,                    // LOGICAL_MAXIMUM (115)
    0x05, 0x07,                    // USAGE_PAGE (Keyboard)
    0x19, 0x00,                    // USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x73,                    // USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    // INPUT (Data,Ary,Abs)
    0xC0,                          // END_COLLECTION

    // Consumer Control descriptor
    0x05, 0x0C,                    // Usage Page (Consumer Devices)
    0x09, 0x01,                    // Usage (Consumer Control)
    0xA1, 0x01,                    // Collection (Application)
    0x85, 0x03,                    // Report ID (3)
    0x15, 0x00,                    // Logical Minimum (0)
    0x26, 0xFF, 0x03,              // Logical Maximum (1023)
    0x19, 0x00,                    // Usage Minimum (0)
    0x2A, 0xFF, 0x03,              // Usage Maximum (1023)
    0x95, 0x04,                    // Report Count (4)
    0x75, 0x10,                    // Report Size (16)
    0x81, 0x00,                    // Input (Data,Array,Abs)
    0xC0                           // End Collection

};
*/

//Uncommented

const uint8_t MinimalKeyboard::HID_REPORT_DESCRIPTOR[] PROGMEM = {
    //  Keyboard
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)  // 47
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
    0xc0,                          // END_COLLECTION

};


MinimalKeyboard::MinimalKeyboard() {
    static HIDSubDescriptor node(HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    HID().AppendDescriptor(&node);
}

void MinimalKeyboard::begin() {}

void MinimalKeyboard::sendReport(KeyReport* report) {
    HID().SendReport(2, report, sizeof(KeyReport));
}

void MinimalKeyboard::sendMediaReport(uint16_t usage) {
    // Create the consumer control HID report
    uint8_t report[3] = {0x03, (uint8_t)usage, (uint8_t)(usage >> 8)};
    HID().SendReport(3, report, sizeof(report));

    // Debugging output
    SerialUSB.print("Media Report Sent: Usage 0x");
    SerialUSB.println(usage, HEX);
}



void MinimalKeyboard::sendTimedMessage(String message, int time) {
  
    // Type out each character
    for (uint8_t i = 0; i < message.length(); i++) {
        char c = message[i];
        uint8_t foundCode = 0;

        // Search the unifiedKeyMap for the matching ASCII value
        for (size_t j = 0; j < unifiedKeyMapSize; j++) {
            KeyInfo k;
            memcpy_P(&k, &unifiedKeyMap[j], sizeof(KeyInfo));
            if (k.asciiValue == c) {
                foundCode = k.hexCode;
                break;
            }
        }

        // Press
        KeyReport press = {};
        press.keys[0] = foundCode;
        sendReport(&press);

        // Release
        KeyReport report = {};  // zero-initialize the struct
        sendReport(&report);
    }

    // Wait the specified amount of time
    delay(time);

    // Press and release Backspace for each character
    for (uint8_t i = 0; i < message.length(); i++) {
        KeyReport pressBackspace{};
        pressBackspace.keys[0] = 0x2A; // Backspace HID code
        sendReport(&pressBackspace);

        KeyReport report = {};  // zero-initialize the struct
        sendReport(&report);
    }
}
