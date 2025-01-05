#ifndef CUSTOM_KEYBOARD_PARSER_H
#define CUSTOM_KEYBOARD_PARSER_H

#include <hidboot.h>
#include "MinimalKeyboard.h"

// Uncommented
class CustomKeyboardParser : public KeyboardReportParser {

public:
    CustomKeyboardParser(MinimalKeyboard& keyboard);
protected:
    void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) override;

private:
    MinimalKeyboard& keyboard;
};


#endif
