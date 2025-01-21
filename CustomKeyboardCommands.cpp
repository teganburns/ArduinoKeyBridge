#include "CustomKeyboardCommands.h"
#include <Arduino.h>  // Assuming you're using Arduino-based libraries

//CustomKeyboardCommands::CustomKeyboardCommands() {}
//CustomKeyboardCommands::~CustomKeyboardCommands() {}

bool CustomKeyboardCommands::COMMAND_MODE = false;

// Singleton accessor
CustomKeyboardCommands& CustomKeyboardCommands::getInstance() {
  static CustomKeyboardCommands instance;  // Guaranteed single instance
  return instance;
}

void CustomKeyboardCommands::init() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("CustomKeyboardCommands initialized.");
#endif
}

void CustomKeyboardCommands::processKey(uint8_t keycode) {

  // This only works if in COMMAND_MODE
  handleKeyPress(keycode);

  switch (keycode) {
    case 0x68: handleF13(); break;  // F13
    case 0x69: handleF14(); break;  // F14
    case 0x6A: handleF15(); break;  // F15
    case 0x6B: handleF16(); break;  // F16
    case 0x6C: handleF17(); break;  // F17
    case 0x6D: handleF18(); break;  // F18
    default:
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
      debugPrint("Unhandled keycode.");
#endif
      break;
  }
}

// If COMMAND_MODE then we process here
void CustomKeyboardCommands::handleKeyPress(uint8_t keycode) {

  /*
  // Print the keycode as a decimal value
  SerialUSB.print("Keycode (Decimal): ");
  SerialUSB.println(keycode);

  // Print the keycode as a hex value
  SerialUSB.print("Keycode (Hex): 0x");
  SerialUSB.println(keycode, HEX);

  // Print it as a character
  SerialUSB.print("Keycode (Char): ");
  SerialUSB.println((char)keycode);
  */



  SerialUSB.print("Keycode: 0x");
  SerialUSB.println(keycode, HEX);

  const KeyInfo* keyInfo = lookupKey(keycode);
  if (keyInfo != nullptr) {
    SerialUSB.println("THE FUCKING KEY WAS FOUND!!!");
    SerialUSB.println(keyInfo->description);

  } else {
    SerialUSB.println("Key not found!");
  }
  SerialUSB.print("CURRENT BUFFER: ");
  SerialUSB.println(convertCommandBufferToASCII());

  if (COMMAND_MODE) {

    // Check if the byte is a newline, return key, or enter key
    if (keycode == 0x28 || keycode == 0x58) {  // scan codes, not ASCII values.
      handleCommand();

      // Clear the buffer by setting all characters to '\0' NULL
      memset(COMMAND_BUFFER_ARRAY, '\0', sizeof(COMMAND_BUFFER_ARRAY));

    } else {
      // Append to the character array if it's not newline or return
      appendKeyInfoToCommandBuffer(keyInfo->hexCode);
    }
  }

#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("end of handleKeyPress()");
#endif
}


void CustomKeyboardCommands::handleF13() {
  debugPrint("F13 pressed.");
  // Custom logic for F13
}

void CustomKeyboardCommands::handleF14() {
  debugPrint("F14 pressed.");
  // Custom logic for F14
}

void CustomKeyboardCommands::handleF15() {
  debugPrint("F15 pressed.");
  // Custom logic for F15
}

void CustomKeyboardCommands::handleF16() {
  debugPrint("F16 pressed.");

  // Turn COMMAND_MODE on/off
  COMMAND_MODE = !COMMAND_MODE;

  // If we are exiting COMMAND_MODE then clear the buffer
  if (!COMMAND_MODE) {
    CustomKeyboardCommands::getInstance().clearCommandBuffer();
    debugPrint("Command Buffer Cleared ");
  }
}

void CustomKeyboardCommands::handleF17() {
  debugPrint("F17 pressed.");
  // Custom logic for F17
}

void CustomKeyboardCommands::handleF18() {
  debugPrint("F18 pressed.");
  // Custom logic for F18
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

// Function to clear the buffer
void CustomKeyboardCommands::clearCommandBuffer() {
  memset(COMMAND_BUFFER_ARRAY, 0, sizeof(COMMAND_BUFFER_ARRAY));  // Clear the buffer
  SerialUSB.println("COMMAND_BUFFER_ARRAY cleared.");
}

// Function to find a key in the key map
const KeyInfo* CustomKeyboardCommands::lookupKey(uint8_t keycode) {
  static KeyInfo tempKeyInfo;  // Static to ensure it persists after function returns
  for (size_t i = 0; i < unifiedKeyMapSize; i++) {
    // Copy the current entry from PROGMEM into a temporary structure
    memcpy_P(&tempKeyInfo, &unifiedKeyMap[i], sizeof(KeyInfo));
    if (tempKeyInfo.hexCode == keycode) {
      return &tempKeyInfo;  // Return a pointer to the temporary structure
    }
  }
  return nullptr;  // Return nullptr if no match is found
}

// Function to append HEX code to the buffer
void CustomKeyboardCommands::appendKeyInfoToCommandBuffer(uint8_t keycode) {
  const KeyInfo* keyInfo = lookupKey(keycode);

  if (keyInfo != nullptr) {
    // Find the next available slot dynamically
    for (size_t i = 0; i < sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0]); i++) {
      if (COMMAND_BUFFER_ARRAY[i].hexCode == 0) {  // Found an empty slot
        COMMAND_BUFFER_ARRAY[i] = *keyInfo;        // Store the KeyInfo
        SerialUSB.print("Appended KeyInfo: Hex Code: 0x");
        SerialUSB.print(keyInfo->hexCode, HEX);
        SerialUSB.print(", ASCII: ");
        if (keyInfo->asciiValue != -1)
          SerialUSB.println((char)keyInfo->asciiValue);
        else
          SerialUSB.println("N/A");
        return;  // Exit after appending
      }
    }
    SerialUSB.println("COMMAND_BUFFER_ARRAY is full.");
  } else {
    SerialUSB.println("Keycode not found in map.");
  }
}

char* CustomKeyboardCommands::convertCommandBufferToASCII() {
  // Calculate the current length dynamically
  size_t length = 0;
  while (length < (sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0])) && COMMAND_BUFFER_ARRAY[length].hexCode != 0) {
    length++;
  }

  // Allocate memory for the ASCII result
  char* asciiResult = (char*)malloc(length + 1);  // +1 for null terminator
  if (asciiResult == nullptr) {
    SerialUSB.println("Memory allocation failed.");
    return nullptr;
  }

  // Convert KeyInfo to ASCII
  for (size_t i = 0; i < length; i++) {
    if (COMMAND_BUFFER_ARRAY[i].asciiValue != -1) {
      asciiResult[i] = (char)COMMAND_BUFFER_ARRAY[i].asciiValue;
    } else {
      asciiResult[i] = '?';  // Placeholder for non-ASCII keys
    }
  }

  asciiResult[length] = '\0';  // Null-terminate
  return asciiResult;
}


void CustomKeyboardCommands::executeCommand() {
  // Ensure the COMMAND_BUFFER_ARRAY contains a valid command
  if (COMMAND_BUFFER_ARRAY[0].hexCode == 0) {
    SerialUSB.println("No command found in buffer.");
    return;
  }

  // Convert the first keycode (or sequence) to a command string
  String command = convertKeyInfoToCommand();

  // Handle commands with a switch-case structure
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

  // Clear the buffer after executing the command
  clearCommandBuffer();
}

String CustomKeyboardCommands::convertKeyInfoToCommand() {
  String command = "";

  // Traverse COMMAND_BUFFER_ARRAY until the first unused slot
  for (size_t i = 0; i < sizeof(COMMAND_BUFFER_ARRAY) / sizeof(COMMAND_BUFFER_ARRAY[0]); i++) {
    if (COMMAND_BUFFER_ARRAY[i].hexCode == 0) {
      break;  // Stop at the first unused slot
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


  // Create a JsonDocument and populate it
  JsonDocument requestDoc;
  requestDoc[""] = "";  // this can be empty for /capture

  // Perform the POST request and retrieve the parsed JsonDocument
  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);


  // Print the parsed "message" field
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

  // Create a JsonDocument and populate it
  JsonDocument requestDoc;
  requestDoc[""] = "";  // this can be empty for /capture

  // Perform the POST request and retrieve the parsed JsonDocument
  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);


  // Print the parsed "message" field
  if (!responseDoc.isNull() && responseDoc.containsKey("message")) {
    SerialUSB.print("Message: ");
    SerialUSB.println(responseDoc["message"].as<const char*>());
  } else {
    SerialUSB.println("Message field not found in response.");
  }
}



void CustomKeyboardCommands::handleSetCommand() {
  SerialUSB.println("Executing Set command...");
  // Add logic for handling "set" here
  // Example: Update a setting or configuration
}
