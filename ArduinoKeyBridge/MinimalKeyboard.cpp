#include "MinimalKeyboard.h"
#include "arduinoKeyBridgeLogger.h"

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

MinimalKeyboard& MinimalKeyboard::getInstance() {
    static MinimalKeyboard instance;
    return instance;
}

MinimalKeyboard::MinimalKeyboard() {
    static HIDSubDescriptor node(HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    HID().AppendDescriptor(&node);
}

void MinimalKeyboard::begin() {}

void MinimalKeyboard::sendReport(KeyReport* report) {
    HID().SendReport(2, report, sizeof(KeyReport));
}

void MinimalKeyboard::sendMediaReport(uint16_t usage) {
    uint8_t report[3] = {0x03, (uint8_t)usage, (uint8_t)(usage >> 8)};
    HID().SendReport(3, report, sizeof(report));

    SerialUSB.print("Media Report Sent: Usage 0x");
    SerialUSB.println(usage, HEX);
}


// Types the message and then deletes it
void MinimalKeyboard::sendTimedMessage(String message, int time) {
    // Type message
    for (uint8_t i = 0; i < message.length(); i++) {
        char c = message[i];
        uint8_t foundCode = 0;

        // Find matching keycode
        for (size_t j = 0; j < unifiedKeyMapSize; j++) {
            KeyInfo k;
            memcpy_P(&k, &unifiedKeyMap[j], sizeof(KeyInfo));
            if (k.asciiValue == c) {
                foundCode = k.hexCode;
                break;
            }
        }

        // Press and release
        KeyReport press = {};
        press.keys[0] = foundCode;
        sendReport(&press);
        
        KeyReport release = {};
        sendReport(&release);
    }

    delay(time);

    // Delete message
    for (uint8_t i = 0; i < message.length(); i++) {
        KeyReport pressBackspace{};
        pressBackspace.keys[0] = 0x2A; // Backspace
        sendReport(&pressBackspace);

        KeyReport release = {};
        sendReport(&release);
    }
}

void MinimalKeyboard::convertMessageToKeyReports() {
    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.clear();
    
    if (KEY_PRESS_MESSAGE == nullptr || KEY_PRESS_MESSAGE[0] == '\0') {
        ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "No message to convert");
        return;
    }

    const char* currentChar = KEY_PRESS_MESSAGE;
    while (*currentChar != '\0') {
        if (*currentChar == '\\' && *(currentChar + 1) != '\0') {
            // Handle escape sequence
            char nextChar = *(currentChar + 1);
            KeyReport keyReport = {0, 0, {0, 0, 0, 0, 0, 0}};
            
            switch (nextChar) {
                case 'n':  // Newline
                    keyReport.keys[0] = 0x28;  // Enter key
                    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.push_back(keyReport);
                    break;
                case 't':  // Tab
                    keyReport.keys[0] = 0x2B;  // Tab key
                    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.push_back(keyReport);
                    break;
                case '\\': // Literal backslash
                    keyReport = convertCharToKeyReport('\\');
                    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.push_back(keyReport);
                    break;
                case '"':  // Double quote
                    keyReport = convertCharToKeyReport('"');
                    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.push_back(keyReport);
                    break;
                default:
                    ArduinoKeyBridgeLogger::getInstance().error("MinimalKeyboard", 
                        "Unknown escape sequence: \\" + String(nextChar));
                    break;
            }
            currentChar += 2;  // Skip both the backslash and the escape character
        } else {
            // Normal character
            KeyReport keyReport = convertCharToKeyReport(*currentChar);
            KEY_PRESS_MESSAGE_AS_KEY_REPORTS.push_back(keyReport);
            currentChar++;
        }
    }
    
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", 
        String("Converted message to ") + String(KEY_PRESS_MESSAGE_AS_KEY_REPORTS.size()) + String(" key reports"));
}

void MinimalKeyboard::setKeyPressMessage(String message) {
    KEY_PRESS_MESSAGE = strdup(message.c_str());
    convertMessageToKeyReports();
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", String("Key press message set to: ") + String(KEY_PRESS_MESSAGE));
}

KeyReport MinimalKeyboard::convertCharToKeyReport(char c) {
    KeyInfo foundCode;
    KeyReport keyReport = {0, 0, {0, 0, 0, 0, 0, 0}};

    // Handle escape sequences
    if (c == '\\') {
        // Get next character from message
        if (KEY_PRESS_MESSAGE != nullptr && KEY_PRESS_MESSAGE[1] != '\0') {
            char nextChar = KEY_PRESS_MESSAGE[1];
            // Remove the extra character from the message since we're processing it now
            KEY_PRESS_MESSAGE = &KEY_PRESS_MESSAGE[1];
            
            // Handle special escape sequences
            switch (nextChar) {
                case 'n':  // Newline
                    keyReport.keys[0] = 0x28;  // Enter key
                    return keyReport;
                case 't':  // Tab
                    keyReport.keys[0] = 0x2B;  // Tab key
                    return keyReport;
                case '\\': // Literal backslash
                    c = '\\';
                    break;
                // Add more escape sequences as needed
                default:
                    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "Unknown escape sequence: \\" + String(nextChar));
                    return keyReport;
            }
        }
    }

    // Search unified key map to find HID code matching this ASCII character
    for (size_t j = 0; j < unifiedKeyMapSize; j++) {
        KeyInfo k;
        memcpy_P(&k, &unifiedKeyMap[j], sizeof(KeyInfo));
        if (k.asciiValue == c) {
            foundCode = k;
            ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", String("Found HID code: 0x") + String(foundCode.hexCode, HEX));
            break;
        }
    }

    if (!foundCode.shifted) {
        keyReport.keys[0] = foundCode.hexCode;
    } else {
        // Shifted key
        keyReport.modifiers = 0x02; // Left Shift
        keyReport.keys[0] = foundCode.hexCode;
    }

    return keyReport;
}

void MinimalKeyboard::sendCharacterFromKeyReportMessage() {
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "Starting sendCharacterFromKeyReportMessage");

    // Check if we have any reports left or if cancel flag is set
    if (KEY_PRESS_MESSAGE_AS_KEY_REPORTS.empty() || KEY_PRESS_CANCEL) {
        ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "No more reports or cancelled - exiting key press mode");
        KEY_PRESS_MESSAGE = nullptr;
        KEY_PRESS = false;
        KEY_PRESS_MODE = false;
        return;
    }

    // Send the press report
    KeyReport keyReport = KEY_PRESS_MESSAGE_AS_KEY_REPORTS.front();
    KEY_PRESS_MESSAGE_AS_KEY_REPORTS.erase(KEY_PRESS_MESSAGE_AS_KEY_REPORTS.begin());
    sendReport(&keyReport);

    // Send the release report
    KeyReport release = {0, 0, {0, 0, 0, 0, 0, 0}};
    sendReport(&release);

    // Log HID report
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", 
        String("HID report: ") + String(keyReport.keys[0], HEX) + String(" ") + 
        String(keyReport.keys[1], HEX) + String(" ") + String(keyReport.keys[2], HEX) + 
        String(" ") + String(keyReport.keys[3], HEX) + String(" ") + 
        String(keyReport.keys[4], HEX) + String(" ") + String(keyReport.keys[5], HEX));

    KEY_PRESS = false;
    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "Press and release reports sent");
}

void MinimalKeyboard::dumpMessage(String message) {
    if (message.length() == 0) {
        ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", "No message to dump (empty message)");
        return;
    }

    ArduinoKeyBridgeLogger::getInstance().debug("MinimalKeyboard", String("Dumping message: ") + message);

    // Type message
    for (uint8_t i = 0; i < message.length(); i++) {
        char c = message[i];
        uint8_t foundCode = 0;

        // Find matching keycode
        for (size_t j = 0; j < unifiedKeyMapSize; j++) {
            KeyInfo k;
            memcpy_P(&k, &unifiedKeyMap[j], sizeof(KeyInfo));
            if (k.asciiValue == c) {
                foundCode = k.hexCode;
                break;
            }
        }

        // Press and release
        KeyReport press = {};
        press.keys[0] = foundCode;
        sendReport(&press);
        
        KeyReport release = {};
        sendReport(&release);
    }
}
