#ifndef MINIMAL_KEYBOARD_H
#define MINIMAL_KEYBOARD_H

#include <HID.h>
#include "Magic_Keyboard_KeyMap.h"

typedef struct {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class MinimalKeyboard {
public:
  MinimalKeyboard();
  void begin();
  void sendReport(KeyReport* report);
  void sendMediaReport(uint16_t usage);
  void sendTimedMessage(String message, int time);

private:
  static const uint8_t HID_REPORT_DESCRIPTOR[];
};

#endif