/*
 * Note - Custom Keyboard commands
 * -----------------
 * 
 * F13 (0x68): Reserved for future use
 * F14 (0x69): Reserved for future use  
 * F15 (0x6A): Reserved for future use
 * F16 (0x6B): Toggle command mode on/off
 * F17 (0x6C): Enter key press mode (active while held)
 * F18 (0x6D): Reserved for future use
 *
 *
 * Command Mode Notes
 * -------------------
 * Command mode is toggled by pressing F16 (0x6B)
 * When active, normal key input is suspended and the following commands are available:
 * - capture - capture a screenshot and send it to the server
 * - send - send screenshots and message to ChatGPT
 * - set - designate custom message to send the server
 *
 * Quick Capture
 * -------------- 
 * F13 (0x68) - Capture a screenshot and send it to the server
 * [IMPLEMENTED - See handleF13() which calls handleCaptureCommand()]
 * 
 * Quick Send
 * --------------
 * F14 (0x69) - Send a message to ChatGPT
 * [IMPLEMENTED - See handleF14() which calls handleSendCommand()]
 * 
 * Quick Message
 * --------------
 * F15 (0x6A) - Set the message to be sent along with screenshots
 * 
 * Key Press Mode Notes
 * -------------------
 * Activated by pressing F17
 * 
 * Synopsis: MinimalKeyboard::getInstance().sendMessageOnPress(String message) takes a message and "types" it out one character at a time. 
 * This function enables keypress mode, allowing the Arduino to simulate keyboard input. 
 * As the user types random letters (A-Z), the Arduino outputs a predefined message or server-retrieved answer, 
 * giving the appearance that the user is typing the message themselves.
 * 
 * When active:
 * - Normal key input is suspended
 * - A message is set by MinimalKeyboard::getInstance().setKeyPressMessage(String message)
 * - Each key press triggers a single character output as determined by the input from MinimalKeyboard::getInstance().sendCharacterFromKeyPressMessage()
 * - Used for character-by-character input control
 * - Deactivates when F17 is pressed again
 */



#include "CustomKeyboardCommands.h"
#include <Arduino.h>

bool CustomKeyboardCommands::COMMAND_MODE = false;

// const char* CHATGPT_PROMPT = "You are taking a Java test. The question may be split across multiple images - review all images before answering. For coding questions, respond only with the complete code solution with no explanations. For multiple choice questions, respond only with the letter of the correct answer (e.g., 'A' or 'B'). Do not include any additional text or explanations in your response.";
const char* CHATGPT_PROMPT = "Ignore the image and tell me a story about satan but use json format.";

CustomKeyboardCommands& CustomKeyboardCommands::getInstance() {
  static CustomKeyboardCommands instance;
  return instance;
}

void CustomKeyboardCommands::init() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "CustomKeyboardCommands initialized.");
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
      ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Unhandled keycode.");
      break;
  }
}

void CustomKeyboardCommands::handleKeyPress(uint8_t keycode) {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Keycode: 0x") + String(keycode, HEX));

  const KeyInfo* keyInfo = lookupKey(keycode);
  if (keyInfo != nullptr) {
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", keyInfo->description);
  } else {
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Key not found!");
  }
  
  if (MinimalKeyboard::getInstance().KEY_PRESS_MODE) {
    MinimalKeyboard::getInstance().KEY_PRESS = true;
    // Remove the return statement to allow key processing to continue
  }
  
  // TODO: COMMAND_MODE and KEY_PRESS_MODE need to be handeled in a more elegant way. Varialbles belong to diffrent classes and functions operate at diffrent places. 
  if (COMMAND_MODE) {
    if (keycode == 0x28 || keycode == 0x58) { // Enter/Return key
      handleCommand();
      memset(COMMAND_BUFFER_ARRAY, '\0', sizeof(COMMAND_BUFFER_ARRAY));
    } else {
      appendKeyInfoToCommandBuffer(keyInfo->hexCode);
    }
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("CURRENT BUFFER: ") + String(convertCommandBufferToASCII()));
  }

  if (MinimalKeyboard::getInstance().KEY_PRESS_MODE) {
    MinimalKeyboard::getInstance().sendCharacterFromKeyReportMessage();
    // This is annoying...
    // ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("CURRENT BUFFER: ") + String(MinimalKeyboard::getInstance().KEY_PRESS_MESSAGE));
  } 

  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "end of handleKeyPress()");
}

void CustomKeyboardCommands::handleF13() {
  handleCaptureCommand();
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F13 pressed.");
}

void CustomKeyboardCommands::handleF14() {
  handleSendCommand();
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F14 pressed.");
}

void CustomKeyboardCommands::handleF15() {
  // TODO: Implement message dump functionality similar to F17 handler but without stealth mode
  // Should use MinimalKeyboard::getInstance().setKeyPressMessage() to output the current message directly
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F15 pressed.");
  
  // Check if we have a stored message
  if (MinimalKeyboard::getInstance().KEY_PRESS_MESSAGE == nullptr) {
    ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "No message available. Use F14 to get a response first.");
    MinimalKeyboard::getInstance().sendTimedMessage("No message", 200);
    return;
  }

  // Output the stored message directly
  String message = String(MinimalKeyboard::getInstance().KEY_PRESS_MESSAGE);
  ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "Outputting message: " + message);
  MinimalKeyboard::getInstance().dumpMessage(message);
}

void CustomKeyboardCommands::handleF16() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F16 pressed.");
  COMMAND_MODE = !COMMAND_MODE;
  
  if (!COMMAND_MODE) {
    CustomKeyboardCommands::getInstance().clearCommandBuffer();
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Command Buffer Cleared");
  }
}

void CustomKeyboardCommands::handleF17() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F17 pressed.");
  
  if (COMMAND_MODE) {
    ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "Cannot start KEY_PRESS_MODE while in COMMAND_MODE");
    return;
  }

  // If KEY_PRESS_MODE is already active, disable it otherwise enable it and set the message
  if (MinimalKeyboard::getInstance().KEY_PRESS_MODE) {
    MinimalKeyboard::getInstance().KEY_PRESS_MODE = false;
    ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "KEY_PRESS_MODE disabled");

  } else {
    // Check if we have a stored message
    if (MinimalKeyboard::getInstance().KEY_PRESS_MESSAGE == nullptr) {
      ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "No message available. Use F14 to get a response first.");
      return;
    }

    // Enable KEY_PRESS_MODE
    MinimalKeyboard::getInstance().KEY_PRESS_MODE = true;
    ArduinoKeyBridgeLogger::getInstance().info("CustomKeyboardCommands", "KEY_PRESS_MODE enabled");

    // Set KEY_PRESS_MESSAGE. This is temporary.
    //MinimalKeyboard::getInstance().setKeyPressMessage("Hello World! This is a test message.");

    //MinimalKeyboard::getInstance().setKeyPressMessage("```python\nprint(\"H   H\")\nprint(\"H   H\")\nprint(\"HHHHH\")\nprint(\"H   H\")\nprint(\"H   H\")\n```");

    // Set KEY_PRESS to true. TODO: I don't think this is needed.
    MinimalKeyboard::getInstance().KEY_PRESS = true;
  }

}

void CustomKeyboardCommands::handleF18() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "F18 pressed.");
}

void CustomKeyboardCommands::handleCommand() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("COMMAND TO BE EXECUTED: ") + String(convertCommandBufferToASCII()));
  executeCommand();
}

void CustomKeyboardCommands::debugPrint(const char* message) {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", message);
}

void CustomKeyboardCommands::clearCommandBuffer() {
  memset(COMMAND_BUFFER_ARRAY, 0, sizeof(COMMAND_BUFFER_ARRAY));
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "COMMAND_BUFFER_ARRAY cleared.");
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
        String logMessage = String("Appended KeyInfo: Hex Code: 0x") + String(keyInfo->hexCode, HEX) + String(", ASCII: ");
        if (keyInfo->asciiValue != -1)
          logMessage += String((char)keyInfo->asciiValue);
        else
          logMessage += "N/A";
        ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", logMessage);
        return;
      }
    }
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "COMMAND_BUFFER_ARRAY is full.");
  } else {
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Keycode not found in map.");
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
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Memory allocation failed.");
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
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "No command found in buffer.");
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
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Unknown command: ") + command);
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
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Executing capture command...");
  const char* resourcePath = "/capture";
  
  MinimalKeyboard::getInstance().sendTimedMessage("capture", 200);

  JsonDocument requestDoc;
  requestDoc[""] = "";

  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);

  if (!responseDoc.isNull() && responseDoc.containsKey("message")) {
    ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Message: ") + responseDoc["message"].as<const char*>());
    MinimalKeyboard::getInstance().sendTimedMessage("Success", 200);
  } else {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Message field not found in response.");
    MinimalKeyboard::getInstance().sendTimedMessage("Error", 200);
  }
}

void CustomKeyboardCommands::handleSendCommand() {

  // IMPORTANT: This function is only for this api call. (/send_request)

  // Log that we're starting the send command process
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Executing send command...");
  const char* resourcePath = "/send_request";

  MinimalKeyboard::getInstance().sendTimedMessage("send_request", 200);

  // Prepare the request JSON document with the specific message
  JsonDocument requestDoc;
  requestDoc["message"] = CHATGPT_PROMPT;

  // Make the POST request to the server and get the response
  JsonDocument responseDoc = WiFiConnection::getInstance().postRequest(serverAddress, serverPort, resourcePath, requestDoc);

  // Check if we received any response at all
  // A null response could indicate network issues or server problems
  if (responseDoc.isNull()) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "No response received");
    MinimalKeyboard::getInstance().sendTimedMessage("Error", 200);
    return;
  } else {
    MinimalKeyboard::getInstance().sendTimedMessage("Success", 200);
  }

  // Navigate through the expected JSON structure:
  // {
  //   "response": {
  //     "choices": [{
  //       "message": {
  //         "content": "actual content here"
  //       }
  //     }]
  //   }
  // }
  
  // First check if we have the "response" object
  if (!responseDoc.containsKey("response")) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Missing 'response' key in response");
    return;
  }

  // Check if response has "choices"
  if (!responseDoc["response"].containsKey("choices")) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Missing 'choices' key in response");
    return;
  }
  
  // Log the size of choices array
  size_t choicesSize = responseDoc["response"]["choices"].size();
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Number of choices: ") + choicesSize);
  
  // Check if choices array has at least one element
  if (choicesSize == 0) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Choices array is empty");
    return;
  }
  
  // Check if first choice has "message"
  if (!responseDoc["response"]["choices"][0].containsKey("message")) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Missing 'message' key in first choice");
    return;
  }
  
  // Check if message has "content"
  if (!responseDoc["response"]["choices"][0]["message"].containsKey("content")) {
    ArduinoKeyBridgeLogger::getInstance().error("CustomKeyboardCommands", "Missing 'content' key in message");
    return;
  }
  
  // If we get here, we have the full structure
  const char* content = responseDoc["response"]["choices"][0]["message"]["content"].as<const char*>();
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Received content: ") + content);
  
  // Store the content in MinimalKeyboard instance
  MinimalKeyboard::getInstance().setKeyPressMessage(content);
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Stored message: ") + content);
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", String("Actual message: ") + MinimalKeyboard::getInstance().KEY_PRESS_MESSAGE);
}

void CustomKeyboardCommands::handleSetCommand() {
  ArduinoKeyBridgeLogger::getInstance().debug("CustomKeyboardCommands", "Executing Set command...");
}


