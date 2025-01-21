#include "CustomKeyboardParser.h"
#include "Magic_Keyboard_KeyMap.h"
#include "CustomKeyboardCommands.h"
#include "ArduinoKeyBridgeLogger.h"

CustomKeyboardParser::CustomKeyboardParser(MinimalKeyboard& kb) : keyboard(kb) {}

void CustomKeyboardParser::Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) {
  // Ensure the report length is valid (at least 8 bytes)
  if (len < 8) return;

  // Initialize a KeyReport structure to store the parsed keyboard report
  KeyReport report = { 0 };
  report.modifiers = buf[1];  // The second byte in the buffer contains modifier keys

  ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", "Key Press Detected");

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

        ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", String("Key Found: Hex Code: 0x") + String(keyInfo.hexCode, HEX) + String(", Description: ") + String(keyInfo.description));

        if (reportIndex < 6) {
          report.keys[reportIndex++] = keyCode;
        }
        break;
      }
    }

    if (!keyFound && reportIndex < 6) {
      ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", String("Key Not Found in Map, Using Raw Hex Code: 0x") + String(keyCode, HEX));
      report.keys[reportIndex++] = keyCode;
    }
  }

  // Send the processed keyboard report to the computer if not in COMMAND_MODE
  if (!CustomKeyboardCommands::getInstance().COMMAND_MODE) {
    keyboard.sendReport(&report);
    ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", "Report sent in normal mode");

    ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", String("Final Report Modifiers: ") + String(report.modifiers, HEX));

    ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", String("Final Report Keys: ") + String(report.keys[0], HEX) + String(" 0x") + String(report.keys[1], HEX) + String(" 0x") + String(report.keys[2], HEX) + String(" 0x") + String(report.keys[3], HEX) + String(" 0x") + String(report.keys[4], HEX) + String(" 0x") + String(report.keys[5], HEX));
  } else {
    ArduinoKeyBridgeLogger::getInstance().debug("KeyboardParser", "COMMAND_MODE ACTIVE");
  }
}

void CustomKeyboardParser::debugPrint(const char* message) {
#if CUSTOM_KEYBOARD_PARSER_DEBUG
  SerialUSB.println(message);
#endif
}