#include "CustomKeyboardCommands.h"
#include <Arduino.h>

bool CustomKeyboardCommands::COMMAND_MODE = false;

CustomKeyboardCommands& CustomKeyboardCommands::getInstance() {
  static CustomKeyboardCommands instance;
  return instance;
}

void CustomKeyboardCommands::init() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("CustomKeyboardCommands initialized.");
#endif
}

void CustomKeyboardCommands::processKey(uint8_t keycode) {
  handleKeyPress(keycode);

  switch (keycode) {
    case 0x68: handleF13(); break;
    case 0x69: handleF14(); break;
    case 0x6A: handleF15(); break;
    case 0x6B: handleF16(); break;
    case 0x6C: handleF17(); break;
    case 0x6D: handleF18(); break;
    default:
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
      debugPrint("Unhandled keycode.");
#endif
      break;
  }
}

void CustomKeyboardCommands::handleKeyPress(uint8_t keycode) {
  SerialUSB.print("Keycode: 0x");
  SerialUSB.println(keycode, HEX);

  const KeyInfo* keyInfo = lookupKey(keycode);
  if (keyInfo != nullptr) {
    SerialUSB.println(keyInfo->description);
  } else {
    SerialUSB.println("Key not found!");
  }
  
  SerialUSB.print("CURRENT BUFFER: ");
  SerialUSB.println(convertCommandBufferToASCII());

  if (COMMAND_MODE) {
    if (keycode == 0x28 || keycode == 0x58) {
      handleCommand();
      memset(COMMAND_BUFFER_ARRAY, '\0', sizeof(COMMAND_BUFFER_ARRAY));
    } else {
      appendKeyInfoToCommandBuffer(keyInfo->hexCode);
    }
  }

#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("end of handleKeyPress()");
#endif
}

void CustomKeyboardCommands::handleF13() {
  debugPrint("F13 pressed.");
}

void CustomKeyboardCommands::handleF14() {
  debugPrint("F14 pressed.");
}

void CustomKeyboardCommands::handleF15() {
  debugPrint("F15 pressed.");
}

void CustomKeyboardCommands::handleF16() {
  debugPrint("F16 pressed.");
  COMMAND_MODE = !COMMAND_MODE;
  
  if (!COMMAND_MODE) {
    CustomKeyboardCommands::getInstance().clearCommandBuffer();
    debugPrint("Command Buffer Cleared ");
  }
}

void CustomKeyboardCommands::handleF17() {
  debugPrint("F17 pressed.");
}

void CustomKeyboardCommands::handleF18() {
  debugPrint("F18 pressed.");
}

void CustomKeyboardCommands::handleCommand() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  SerialUSB.print("COMMAND TO BE EXECUTED: ");
  SerialUSB.println(convertCommandBufferToASCII());
#endif
  executeCommand();
}

void CustomKeyboardCommands::debugPrint(const char* message) {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  SerialUSB.println(message);
#endif
}

void CustomKeyboardCommands::clearCommandBuffer() {
  memset(COMMAND_BUFFER_ARRAY, 0, sizeof(COMMAND_BUFFER_ARRAY));
  SerialUSB.println("COMMAND_BUFFER_ARRAY cleared.");
}

const KeyInfo* CustomKeyboardCommands::lookupKey(uint8_t keycode) {
  static KeyInfo tempKeyInfo;
  for (size_t i = 0; i < unifiedKeyMapSize; i++) {
    memcpy_P(&tempKeyInfo, &unifiedKeyMap[i], sizeof(KeyInfo));
    if (tempKeyInfo.hexCode == keycode) {
      return &tempKeyInfo;
    }
  }
  return nullptr;
}

void CustomKeyboardCommands::appendKeyInfoToCommandBuffer(uint8_t keycode) {
  const KeyInfo* keyInfo = lookupKey(keycode);

  if (keyInfo != nullptr) {
    for (size_t i = 0; i < sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0]); i++) {
      if (COMMAND_BUFFER_ARRAY[i].hexCode == 0) {
        COMMAND_BUFFER_ARRAY[i] = *keyInfo;
        SerialUSB.print("Appended KeyInfo: Hex Code: 0x");
        SerialUSB.print(keyInfo->hexCode, HEX);
        SerialUSB.print(", ASCII: ");
        if (keyInfo->asciiValue != -1)
          SerialUSB.println((char)keyInfo->asciiValue);
        else
          SerialUSB.println("N/A");
        return;
      }
    }
    SerialUSB.println("COMMAND_BUFFER_ARRAY is full.");
  } else {
    SerialUSB.println("Keycode not found in map.");
  }
}

char* CustomKeyboardCommands::convertCommandBufferToASCII() {
  size_t length = 0;
  while (length < (sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0])) && 
         COMMAND_BUFFER_ARRAY[length].hexCode != 0) {
    length++;
  }

  char* asciiResult = (char*)malloc(length + 1);
  if (asciiResult == nullptr) {
    SerialUSB.println("Memory allocation failed.");
    return nullptr;
  }

  for (size_t i = 0; i < length; i++) {
    asciiResult[i] = (COMMAND_BUFFER_ARRAY[i].asciiValue != -1) ? 
                     (char)COMMAND_BUFFER_ARRAY[i].asciiValue : '?';
  }

  asciiResult[length] = '\0';
  return asciiResult;
}

void CustomKeyboardCommands::executeCommand() {
  if (COMMAND_BUFFER_ARRAY[0].hexCode == 0) {
    SerialUSB.println("No command found in buffer.");
    return;
  }

  String command = convertKeyInfoToCommand();

  if (command == "capture") {
    handleCaptureCommand();
  } else if (command == "send") {
    handleSendCommand();
  } else if (command == "set") {
    handleSetCommand();
  } else {
    SerialUSB.print("Unknown command: ");
    SerialUSB.println(command);
  }

  clearCommandBuffer();
}

String CustomKeyboardCommands::convertKeyInfoToCommand() {
  String command = "";

  for (size_t i = 0; i < sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0]); i++) {
    if (COMMAND_BUFFER_ARRAY[i].hexCode == 0) {
      break;
    }
    if (COMMAND_BUFFER_ARRAY[i].asciiValue != -1) {
      command += (char)COMMAND_BUFFER_ARRAY[i].asciiValue;
    }
  }

  return command;
}

void CustomKeyboardCommands::handleCaptureCommand() {
  SerialUSB.println("Executing capture command...");
  const char* resourcePath = "/capture";
  MinimalKeyboard kb;
  kb.sendTimedMessage("capture", 200);

  JsonDocument requestDoc;
  requestDoc[""] = "";

  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);

  if (!responseDoc.isNull() && responseDoc.containsKey("message")) {
    SerialUSB.print("Message: ");
    SerialUSB.println(responseDoc["message"].as<const char*>());
  } else {
    SerialUSB.println("Message field not found in response.");
  }
}

void CustomKeyboardCommands::handleSendCommand() {
  SerialUSB.println("Executing send command...");
  const char* resourcePath = "/send_request";

  JsonDocument requestDoc;
  requestDoc[""] = "";

  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);

  if (!responseDoc.isNull() && responseDoc.containsKey("message")) {
    SerialUSB.print("Message: ");
    SerialUSB.println(responseDoc["message"].as<const char*>());
  } else {
    SerialUSB.println("Message field not found in response.");
  }
}

void CustomKeyboardCommands::handleSetCommand() {
  SerialUSB.println("Executing Set command...");
}
