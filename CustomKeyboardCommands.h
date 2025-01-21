#ifndef CUSTOM_KEYBOARD_COMMANDS_H
#define CUSTOM_KEYBOARD_COMMANDS_H

#include <stdint.h>
#include <Arduino.h>
#include "JsonHandler.h"
#include "WiFiConnection.h"
#include "MinimalKeyboard.h"

#include "Magic_Keyboard_KeyMap.h"


// Define debugging flag
#ifndef CUSTOM_KEYBOARD_COMMANDS_DEBUG
#define CUSTOM_KEYBOARD_COMMANDS_DEBUG true
#endif

class CustomKeyboardCommands {
public:

  static CustomKeyboardCommands& getInstance();  // Singleton accessor

  static bool COMMAND_MODE;           // Tracks whether command mode is active
  KeyInfo COMMAND_BUFFER_ARRAY[512];  // Buffer keypresses while in command mode

  // Constructor and Destructor
  //CustomKeyboardCommands();
  //~CustomKeyboardCommands();

  // Method to initialize the command listener
  void init();

  // Method to process a keypress
  void processKey(uint8_t keycode);

  void handleKeyPress(uint8_t keycode);


private:

  const char* serverAddress = "10.149.1.141";
  int serverPort = 5001;

  // Helper methods for specific key handling
  void handleF13();
  void handleF14();
  void handleF15();
  void handleF16();
  void handleF17();
  void handleF18();

  void handleCommand();


  // Debug helper method
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


  CustomKeyboardCommands() = default;   // Private constructor
  ~CustomKeyboardCommands() = default;  // Private destructor

  // Delete copy constructor and assignment operator
  CustomKeyboardCommands(const CustomKeyboardCommands&) = delete;  // Prevent copy
  CustomKeyboardCommands& operator=(const CustomKeyboardCommands&) = delete;
};

#endif  // CUSTOM_KEYBOARD_COMMANDS_H