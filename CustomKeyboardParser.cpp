#include "CustomKeyboardParser.h"
#include "Magic_Keyboard_KeyMap.h"
#include "CustomKeyboardCommands.h"


CustomKeyboardParser::CustomKeyboardParser(MinimalKeyboard& kb) : keyboard(kb) {}

void CustomKeyboardParser::Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) {

  // Ensure the report length is valid (at least 8 bytes)
  if (len < 8) return;

  // Initialize a KeyReport structure to store the parsed keyboard report
  KeyReport report = { 0 };
  report.modifiers = buf[1];  // The second byte in the buffer contains modifier keys
  

#if CUSTOM_KEYBOARD_PARSER_DEBUG
  SerialUSB.println("---- Key Press Detected ----");
  //SerialUSB.print("Modifiers (Raw): ");
  //SerialUSB.println(buf, HEX);  // TODO: Print the raw modifier byte in hexadecimal
#endif

  int reportIndex = 0;  // Index to track where to place keys in the KeyReport

  // Iterate through the buffer to process key codes (starting at the third byte)
  for (int i = 2; i < 8; i++) {
    uint8_t keyCode = buf[i];

    // Skip if no key is pressed in this slot (keyCode == 0)
    if (keyCode == 0) continue;

    //CustomKeyboardCommands::getInstance().processKey(keyCode);


    // Look up the key code in the unified key map
    bool keyFound = false;  // Track if the key code was found
    for (size_t j = 0; j < unifiedKeyMapSize; j++) {
      KeyInfo keyInfo;

      // Copy key information from program memory (PROGMEM) to RAM
      memcpy_P(&keyInfo, &unifiedKeyMap[j], sizeof(KeyInfo));

      // Check if the current key code matches the hexCode in the key map
      if (keyInfo.hexCode == keyCode) {
        keyFound = true;
        CustomKeyboardCommands::getInstance().processKey(keyCode);
#if CUSTOM_KEYBOARD_PARSER_DEBUG
        // Log the key information
        SerialUSB.print("Key Found: ");
        SerialUSB.print("Hex Code: 0x");
        SerialUSB.print(keyInfo.hexCode, HEX);
        SerialUSB.print(", Description: ");
        SerialUSB.println(keyInfo.description);
#endif

        // Add the key code to the report, even if it has no ASCII value
        if (reportIndex < 6) {
          report.keys[reportIndex++] = keyCode;
        }
        break;  // Exit the loop once the key is found
      }
    }

    if (!keyFound) {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
      // If the key is not found in the map, use the raw hex value directly
      SerialUSB.print("Key Not Found in Map, Using Raw Hex Code: 0x");
      SerialUSB.println(keyCode, HEX);
#endif

      // Add the raw hex code to the report if there's space
      if (reportIndex < 6) {
        report.keys[reportIndex++] = keyCode;
      }
    }
  }


  // Send the processed keyboard report to the computer if not in COMMAND_MODE
  if (!CustomKeyboardCommands::getInstance().COMMAND_MODE) {
    keyboard.sendReport(&report);

    // Log the final state of the keyboard report
    SerialUSB.print("Final Report Modifiers: ");
    SerialUSB.println(report.modifiers, HEX);

    debugPrint("Final Report Keys: ");
    for (int i = 0; i < 6; i++) {
      SerialUSB.print("0x");
      SerialUSB.print(report.keys[i], HEX);
      SerialUSB.print(" ");
    }
    SerialUSB.println("\n---------------------------");  // Separator for better readability in logs

  } else {
    // Command mode is active, skip sending the report // Custom handling can occur in CustomKeyboardCommands::ProcessKey
    SerialUSB.print("COMMAND_MODE ACTIVE");  // Separator for better readability in logs
    SerialUSB.println("\n---------------------------");  // Separator for better readability in logs

  }
}

void CustomKeyboardParser::debugPrint(const char* message) {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
  SerialUSB.println(message);
#endif
}