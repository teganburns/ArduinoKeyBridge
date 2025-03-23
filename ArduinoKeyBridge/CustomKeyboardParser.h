#ifndef CUSTOM_KEYBOARD_PARSER_H
#define CUSTOM_KEYBOARD_PARSER_H

#ifndef CUSTOM_KEYBOARD_PARSER_DEBUG
#define CUSTOM_KEYBOARD_PARSER_DEBUG true
#endif

#include <hidboot.h>
#include "MinimalKeyboard.h"
#include "ArduinoKeyBridgeLogger.h"

class CustomKeyboardParser : public KeyboardReportParser {
public:
    CustomKeyboardParser(MinimalKeyboard& keyboard);
    
protected:
    void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) override;

private:
    MinimalKeyboard& keyboard;
    void debugPrint(const char* message);
};

#endif
