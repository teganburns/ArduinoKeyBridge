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
#endif

  int reportIndex = 0;  // Index to track where to place keys in the KeyReport

  // Iterate through the buffer to process key codes (starting at the third byte)
  for (int i = 2; i < 8; i++) {
    uint8_t keyCode = buf[i];
    if (keyCode == 0) continue; // Skip if no key is pressed in this slot

    // Look up the key code in the unified key map
    bool keyFound = false;
    for (size_t j = 0; j < unifiedKeyMapSize; j++) {
      KeyInfo keyInfo;
      memcpy_P(&keyInfo, &unifiedKeyMap[j], sizeof(KeyInfo));

      if (keyInfo.hexCode == keyCode) {
        keyFound = true;
        CustomKeyboardCommands::getInstance().processKey(keyCode);

#if CUSTOM_KEYBOARD_PARSER_DEBUG
        SerialUSB.print("Key Found: Hex Code: 0x");
        SerialUSB.print(keyInfo.hexCode, HEX);
        SerialUSB.print(", Description: ");
        SerialUSB.println(keyInfo.description);
#endif

        if (reportIndex < 6) {
          report.keys[reportIndex++] = keyCode;
        }
        break;
      }
    }

    if (!keyFound && reportIndex < 6) {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
      SerialUSB.print("Key Not Found in Map, Using Raw Hex Code: 0x");
      SerialUSB.println(keyCode, HEX);
#endif
      report.keys[reportIndex++] = keyCode;
    }
  }

  // Send the processed keyboard report to the computer if not in COMMAND_MODE
  if (!CustomKeyboardCommands::getInstance().COMMAND_MODE) {
    keyboard.sendReport(&report);

#if CUSTOM_KEYBOARD_PARSER_DEBUG
    SerialUSB.print("Final Report Modifiers: ");
    SerialUSB.println(report.modifiers, HEX);

    SerialUSB.print("Final Report Keys: ");
    for (int i = 0; i < 6; i++) {
      SerialUSB.print("0x");
      SerialUSB.print(report.keys[i], HEX);
      SerialUSB.print(" ");
    }
    SerialUSB.println("\n---------------------------");
#endif

  } else {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
    SerialUSB.println("COMMAND_MODE ACTIVE");
    SerialUSB.println("---------------------------");
#endif
  }
}

void CustomKeyboardParser::debugPrint(const char* message) {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
  SerialUSB.println(message);
#endif
}