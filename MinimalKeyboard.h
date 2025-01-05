#ifndef MINIMAL_KEYBOARD_H
#define MINIMAL_KEYBOARD_H

#include <HID.h>

//Uncommented
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
    void sendMediaReport(uint16_t usage); // Declare sendMediaReport function

private:
    static const uint8_t HID_REPORT_DESCRIPTOR[]; // Declare the descriptor here
};

#endif
