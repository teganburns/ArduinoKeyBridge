#ifndef MAGIC_KEYBOARD_KEYMAP_H
#define MAGIC_KEYBOARD_KEYMAP_H

#include <avr/pgmspace.h>

// Define a struct to hold key information
struct KeyInfo {
    uint8_t hexCode;         // Hexadecimal keycode
    int asciiValue;          // ASCII value (or -1 if not applicable)
    const char* description; // Key description
};
const KeyInfo unifiedKeyMap[] PROGMEM = {

    // Alphanumeric Keys
    {0x04, 'a', "a"}, {0x05, 'b', "b"}, {0x06, 'c', "c"}, {0x07, 'd', "d"},
    {0x08, 'e', "e"}, {0x09, 'f', "f"}, {0x0A, 'g', "g"}, {0x0B, 'h', "h"},
    {0x0C, 'i', "i"}, {0x0D, 'j', "j"}, {0x0E, 'k', "k"}, {0x0F, 'l', "l"},
    {0x10, 'm', "m"}, {0x11, 'n', "n"}, {0x12, 'o', "o"}, {0x13, 'p', "p"},
    {0x14, 'q', "q"}, {0x15, 'r', "r"}, {0x16, 's', "s"}, {0x17, 't', "t"},
    {0x18, 'u', "u"}, {0x19, 'v', "v"}, {0x1A, 'w', "w"}, {0x1B, 'x', "x"},
    {0x1C, 'y', "y"}, {0x1D, 'z', "z"},

    // Numbers (Top Row)
    {0x1E, '1', "1"}, {0x1F, '2', "2"}, {0x20, '3', "3"}, {0x21, '4', "4"},
    {0x22, '5', "5"}, {0x23, '6', "6"}, {0x24, '7', "7"}, {0x25, '8', "8"},
    {0x26, '9', "9"}, {0x27, '0', "0"},

    // Numeric Keypad
    {0x62, '0', "0 (Numpad)"},
    {0x59, '1', "1 (Numpad)"},
    {0x5A, '2', "2 (Numpad)"},
    {0x5B, '3', "3 (Numpad)"},
    {0x5C, '4', "4 (Numpad)"},
    {0x5D, '5', "5 (Numpad)"},
    {0x5E, '6', "6 (Numpad)"},
    {0x5F, '7', "7 (Numpad)"},
    {0x60, '8', "8 (Numpad)"},
    {0x61, '9', "9 (Numpad)"},
    {0x58, -1, "Enter (Numpad)"},
    {0x54, '/', "Divide (Numpad)"},
    {0x55, '*', "Multiply (Numpad)"},
    {0x56, '-', "Subtract (Numpad)"},
    {0x57, '+', "Add (Numpad)"},
    {0x63, '.', "Decimal (Numpad)"},
    {0x67, '=', "Equal (Numpad)"},

    // Symbol Keys
    {0x2D, '-', "Hyphen"},
    {0x2E, '=', "EqualSign"},
    {0x2F, '[', "OpenBracket"},
    {0x30, ']', "CloseBracket"},
    {0x31, '\\', "Backslash"},
    {0x33, ';', "Semicolon"},
    {0x34, '\'', "Quote"},
    {0x35, '`', "GraveAccentAndTilde"},
    {0x36, ',', "Comma"},
    {0x37, '.', "Period"},
    {0x38, '/', "Slash"},
    {0x32, -1, "NonUSPound"},
    {0x64, -1, "NonUSBackslash"},

    {0x1E, '!', "!"},  // Shift + 1 = !
    {0x1F, '@', "@"},  // Shift + 2 = @
    {0x20, '#', "#"},  // Shift + 3 = #
    {0x21, '$', "$"},  // Shift + 4 = $
    {0x22, '%', "%"},  // Shift + 5 = %
    {0x23, '^', "^"},  // Shift + 6 = ^
    {0x24, '&', "&"},  // Shift + 7 = &
    {0x25, '*', "*"},  // Shift + 8 = *
    {0x26, '(', "("},  // Shift + 9 = (
    {0x27, ')', ")"},  // Shift + 0 = )

    // Special Keys
    {0x2C, ' ', "Spacebar"},
    {0x28, '\n', "Enter"},
    {0x58, '\n', "Enter (Bottom Right)"},  // Added bottom-right Enter key
    {0x29, 27, "Escape"},        // 27 = ESC
    {0x2A, 8, "Backspace"},      // 8 = BS
    {0x2B, '\t', "Tab"},         // 9 = Tab
    {0x4C, -1, "Delete"},
    {0x4C, -1, "Backward Delete"}, // Added Backward Delete key
    {0x52, -1, "Arrow Up"},
    {0x51, -1, "Arrow Down"},
    {0x50, -1, "Arrow Left"},
    {0x4F, -1, "Arrow Right"},
    {0x4A, -1, "Home"},
    {0x4B, -1, "PageUp"},
    {0x4D, -1, "End"},
    {0x4E, -1, "PageDown"},
    {0x49, -1, "Insert"},

    // Function Keys
    {0x3A, -1, "F1"},
    {0x3B, -1, "F2"},
    {0x3C, -1, "F3"},
    {0x3D, -1, "F4"},
    {0x3E, -1, "F5"},
    {0x3F, -1, "F6"},
    {0x40, -1, "F7"},
    {0x41, -1, "F8"},
    {0x42, -1, "F9"},
    {0x43, -1, "F10"},
    {0x44, -1, "F11"},
    {0x45, -1, "F12"},
    {0x68, -1, "F13"},
    {0x69, -1, "F14"},
    {0x6A, -1, "F15"},
    {0x6B, -1, "F16"},
    {0x6C, -1, "F17"},
    {0x6D, -1, "F18"},
    {0x6E, -1, "F19"},
    {0x6F, -1, "F20"},
    {0x70, -1, "F21"},
    {0x71, -1, "F22"},
    {0x72, -1, "F23"},
    {0x73, -1, "F24"},

    // Other / System Keys
    {0x01, -1, "ErrorRollOver"},
    {0x03, -1, "ErrorUndefined"},
    {0x79, -1, "Again"},
    {0x99, -1, "AlternateErase"},
    {0x65, -1, "Application"},
    {0x9B, -1, "Cancel"},
    {0x9C, -1, "Clear"},
    {0xA2, -1, "ClearOrAgain"},
    {0x7C, -1, "Copy"},
    {0xA3, -1, "CrSelOrProps"},
    {0x7B, -1, "Cut"},
    {0xA4, -1, "ExSel"},
    {0x74, -1, "Execute"},
    {0x7E, -1, "Find"},
    {0x75, -1, "Help"},
    {0x76, -1, "Menu"},
    {0xA1, -1, "Oper"},
    {0xA0, -1, "Out"},
    {0x02, -1, "POSTFail"},
    {0x7D, -1, "Paste"},
    {0x66, -1, "Power"},
    {0x46, -1, "PrintScreen"},
    {0x9D, -1, "Prior"},
    {0x77, -1, "Select"},
    {0x9A, -1, "SysReqOrAttention"},
    {0x7A, -1, "Undo"}

};

// Number of entries in the unified key map
constexpr size_t unifiedKeyMapSize = sizeof(unifiedKeyMap) / sizeof(KeyInfo);

#endif // MAGIC_KEYBOARD_KEYMAP_H