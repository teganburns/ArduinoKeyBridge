#ifndef MINIMAL_KEYBOARD_H
#define MINIMAL_KEYBOARD_H

#include <hidboot.h>
#include <HID.h>
#include "MagicKeyboardKeyMap.h"
#include "ArduinoKeyBridgeLogger.h"

// Key report structure
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
    void onNewKeyReport(const uint8_t* buf, uint8_t len);

    // Flag to indicate new report available
    bool hasNewReport = false;
    KeyReport currentReport;

private:
    MinimalKeyboard();  // Private constructor
    static const uint8_t HID_REPORT_DESCRIPTOR[];
    MinimalKeyboard(const MinimalKeyboard&) = delete;
    MinimalKeyboard& operator=(const MinimalKeyboard&) = delete;
};

class MinimalKeyboardParser : public KeyboardReportParser {
public:
    MinimalKeyboardParser(MinimalKeyboard& keyboard) : keyboard_(keyboard) {}
protected:
    void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) override {
        keyboard_.onNewKeyReport(buf, len);
    }
private:
    MinimalKeyboard& keyboard_;
};

#endif