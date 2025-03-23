#ifndef CUSTOM_KEYBOARD_COMMANDS_H
#define CUSTOM_KEYBOARD_COMMANDS_H

#include <stdint.h>
#include <Arduino.h>
#include "JsonHandler.h"
#include "WiFiConnection.h"
#include "MinimalKeyboard.h"
#include "Magic_Keyboard_KeyMap.h"

#ifndef CUSTOM_KEYBOARD_COMMANDS_DEBUG
#define CUSTOM_KEYBOARD_COMMANDS_DEBUG true
#endif

class CustomKeyboardCommands {
public:
  static CustomKeyboardCommands& getInstance();
  static bool COMMAND_MODE;
  KeyInfo COMMAND_BUFFER_ARRAY[512];

  void init();
  void processKey(uint8_t keycode);
  void handleKeyPress(uint8_t keycode);

private:
  const char* serverAddress = "10.149.1.141";
  int serverPort = 5001;

  void handleF13();
  void handleF14();
  void handleF15();
  void handleF16();
  void handleF17();
  void handleF18();
  void handleCommand();

  void debugPrint(const char* message);
  const KeyInfo* lookupKey(uint8_t keycode);
  void appendKeyInfoToCommandBuffer(uint8_t keycode);
  void clearCommandBuffer();
  char* convertCommandBufferToASCII();
  void executeCommand();
  String convertKeyInfoToCommand();

  void handleCaptureCommand();
  void handleSendCommand();
  void handleSetCommand();

  CustomKeyboardCommands() = default;
  ~CustomKeyboardCommands() = default;
  CustomKeyboardCommands(const CustomKeyboardCommands&) = delete;
  CustomKeyboardCommands& operator=(const CustomKeyboardCommands&) = delete;
};

#endif