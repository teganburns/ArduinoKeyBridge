#include "CustomKeyboardParser.h"
#include "Magic_Keyboard_KeyMap.h"

//Uncommented

CustomKeyboardParser::CustomKeyboardParser(MinimalKeyboard& kb) : keyboard(kb) {}

void CustomKeyboardParser::Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) {
    SerialUSB.print("Len: ");
    SerialUSB.println(len);
    
    // Ensure the report length is valid (at least 8 bytes)
    if (len < 8) return;

    // Initialize a KeyReport structure to store the parsed keyboard report
    KeyReport report = {0};
    report.modifiers = buf[1]; // The second byte in the buffer contains modifier keys

    // Log that a key press has been detected
    SerialUSB.println("---- Key Press Detected ----");
    SerialUSB.print("Modifiers (Raw): ");
    SerialUSB.println(report.modifiers, HEX); // Print the raw modifier byte in hexadecimal

    int reportIndex = 0; // Index to track where to place keys in the KeyReport

    // Iterate through the buffer to process key codes (starting at the third byte)
    for (int i = 2; i < 8; i++) {
        uint8_t keyCode = buf[i];

        // Skip if no key is pressed in this slot (keyCode == 0)
        if (keyCode == 0) continue;

        // Look up the key code in the unified key map
        bool keyFound = false; // Track if the key code was found
        for (size_t j = 0; j < unifiedKeyMapSize; j++) {
            KeyInfo keyInfo;

            // Copy key information from program memory (PROGMEM) to RAM
            memcpy_P(&keyInfo, &unifiedKeyMap[j], sizeof(KeyInfo));

            // Check if the current key code matches the hexCode in the key map
            if (keyInfo.hexCode == keyCode) {
                keyFound = true;

                // Log the key information
                SerialUSB.print("Key Found: ");
                SerialUSB.print("Hex Code: 0x");
                SerialUSB.print(keyInfo.hexCode, HEX);
                SerialUSB.print(", Description: ");
                SerialUSB.println(keyInfo.description);

                // Add the key code to the report, even if it has no ASCII value
                if (reportIndex < 6) {
                    report.keys[reportIndex++] = keyCode;
                }
                break; // Exit the loop once the key is found
            }
        }

        if (!keyFound) {
            // If the key is not found in the map, use the raw hex value directly
            SerialUSB.print("Key Not Found in Map, Using Raw Hex Code: 0x");
            SerialUSB.println(keyCode, HEX);

            // Add the raw hex code to the report if there's space
            if (reportIndex < 6) {
                report.keys[reportIndex++] = keyCode;
            }
        }
    }

    // Send the processed keyboard report to the computer
    keyboard.sendReport(&report);

    // Log the final state of the keyboard report
    SerialUSB.print("Final Report Modifiers: ");
    SerialUSB.println(report.modifiers, HEX);

    SerialUSB.print("Final Report Keys: ");
    for (int i = 0; i < 6; i++) {
        SerialUSB.print("0x");
        SerialUSB.print(report.keys[i], HEX);
        SerialUSB.print(" ");
    }
    Serial.println("\n---------------------------"); // Separator for better readability in logs
}