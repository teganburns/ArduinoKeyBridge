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

void CustomKeyboardCommands::handleF13() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F13 pressed.");
#endif
  // Custom logic for F13
}

void CustomKeyboardCommands::handleF14() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F14 pressed.");
#endif
  // Custom logic for F14
}

void CustomKeyboardCommands::handleF15() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F15 pressed.");
#endif
  // Custom logic for F15
}

void CustomKeyboardCommands::handleF16() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F16 pressed.");
#endif
  // Custom logic for F16
  // Turn COMMAND_MODE on/off
  COMMAND_MODE = !COMMAND_MODE;
}

void CustomKeyboardCommands::handleF17() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F17 pressed.");
#endif
  // Custom logic for F17
}

void CustomKeyboardCommands::handleF18() {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  debugPrint("F18 pressed.");
#endif
  // Custom logic for F18
}

void CustomKeyboardCommands::debugPrint(const char* message) {
#if CUSTOM_KEYBOARD_COMMANDS_DEBUG
  SerialUSB.println(message);
#endif
}