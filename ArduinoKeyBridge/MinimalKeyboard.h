#ifndef MINIMAL_KEYBOARD_H
#define MINIMAL_KEYBOARD_H

#include <Arduino.h>
#include <HID.h>
#include <vector>
#include "Magic_Keyboard_KeyMap.h"
#include "ArduinoKeyBridgeLogger.h"

typedef struct {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class MinimalKeyboard {
public:
  static MinimalKeyboard& getInstance();
  void begin();
  void sendReport(KeyReport* report);
  void sendMediaReport(uint16_t usage);
  void sendTimedMessage(String message, int time);
  void sendCharacterFromKeyPressMessage();

  // Dumb functions to set and cancel key press message
  void setKeyPressMessage(String message);
  void cancelKeyPressMessage();

  void sendCharacterFromKeyReportMessage();

  bool KEY_PRESS = false;
  bool KEY_PRESS_CANCEL = false;
  bool KEY_PRESS_MODE = false;
  char* KEY_PRESS_MESSAGE = nullptr;

  void dumpMessage(String message); // Function to dump the given message

private:
  MinimalKeyboard();  // Private constructor
  static const uint8_t HID_REPORT_DESCRIPTOR[];
  std::vector<KeyReport> KEY_PRESS_MESSAGE_AS_KEY_REPORTS;
  void convertMessageToKeyReports();
  KeyReport convertCharToKeyReport(char c);
};

#endif