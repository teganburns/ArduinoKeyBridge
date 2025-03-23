#ifndef MAGIC_KEYBOARD_KEYMAP_H
#define MAGIC_KEYBOARD_KEYMAP_H

#include <avr/pgmspace.h>

// Define a struct to hold key information
struct KeyInfo {
    uint8_t hexCode;         // Hexadecimal keycode
    int asciiValue;          // ASCII value (or -1 if not applicable) 
    const char* description; // Key description
    bool shifted;           // Does they keycode need to be shifted?
};

const KeyInfo unifiedKeyMap[] PROGMEM = {
    // Alphanumeric Keys - Lowercase
    {0x04, 'a', "a", false}, {0x05, 'b', "b", false}, {0x06, 'c', "c", false}, {0x07, 'd', "d", false},
    {0x08, 'e', "e", false}, {0x09, 'f', "f", false}, {0x0A, 'g', "g", false}, {0x0B, 'h', "h", false},
    {0x0C, 'i', "i", false}, {0x0D, 'j', "j", false}, {0x0E, 'k', "k", false}, {0x0F, 'l', "l", false},
    {0x10, 'm', "m", false}, {0x11, 'n', "n", false}, {0x12, 'o', "o", false}, {0x13, 'p', "p", false},
    {0x14, 'q', "q", false}, {0x15, 'r', "r", false}, {0x16, 's', "s", false}, {0x17, 't', "t", false},
    {0x18, 'u', "u", false}, {0x19, 'v', "v", false}, {0x1A, 'w', "w", false}, {0x1B, 'x', "x", false},
    {0x1C, 'y', "y", false}, {0x1D, 'z', "z", false},

    // Alphanumeric Keys - Uppercase
    {0x04, 'A', "A", true}, {0x05, 'B', "B", true}, {0x06, 'C', "C", true}, {0x07, 'D', "D", true},
    {0x08, 'E', "E", true}, {0x09, 'F', "F", true}, {0x0A, 'G', "G", true}, {0x0B, 'H', "H", true},
    {0x0C, 'I', "I", true}, {0x0D, 'J', "J", true}, {0x0E, 'K', "K", true}, {0x0F, 'L', "L", true},
    {0x10, 'M', "M", true}, {0x11, 'N', "N", true}, {0x12, 'O', "O", true}, {0x13, 'P', "P", true},
    {0x14, 'Q', "Q", true}, {0x15, 'R', "R", true}, {0x16, 'S', "S", true}, {0x17, 'T', "T", true},
    {0x18, 'U', "U", true}, {0x19, 'V', "V", true}, {0x1A, 'W', "W", true}, {0x1B, 'X', "X", true},
    {0x1C, 'Y', "Y", true}, {0x1D, 'Z', "Z", true},

    // Numbers (Top Row)
    {0x1E, '1', "1", false}, {0x1F, '2', "2", false}, {0x20, '3', "3", false}, {0x21, '4', "4", false},
    {0x22, '5', "5", false}, {0x23, '6', "6", false}, {0x24, '7', "7", false}, {0x25, '8', "8", false}, 
    {0x26, '9', "9", false}, {0x27, '0', "0", false},

    // Numeric Keypad
    {0x59, '1', "1 (Numpad)", false}, {0x5A, '2', "2 (Numpad)", false}, {0x5B, '3', "3 (Numpad)", false},
    {0x5C, '4', "4 (Numpad)", false}, {0x5D, '5', "5 (Numpad)", false}, {0x5E, '6', "6 (Numpad)", false},
    {0x5F, '7', "7 (Numpad)", false}, {0x60, '8', "8 (Numpad)", false}, {0x61, '9', "9 (Numpad)", false},
    {0x62, '0', "0 (Numpad)", false}, {0x63, '.', "Decimal (Numpad)", false}, {0x67, '=', "Equal (Numpad)", false},
    {0x58, -1, "Enter (Numpad)", false}, {0x54, '/', "Divide (Numpad)", false}, {0x55, '*', "Multiply (Numpad)", false},
    {0x56, '-', "Subtract (Numpad)", false}, {0x57, '+', "Add (Numpad)", false},

    // Symbol Keys
    {0x2D, '-', "Hyphen", false}, {0x2E, '=', "Equal Sign", false}, {0x2F, '[', "Open Bracket", false},
    {0x30, ']', "Close Bracket", false}, {0x31, '\\', "Backslash", false}, {0x33, ';', "Semicolon", false},
    {0x34, '\'', "Quote", false}, {0x35, '`', "Grave Accent", false}, {0x36, ',', "Comma", false},
    {0x37, '.', "Period", false}, {0x38, '/', "Slash", false}, {0x32, -1, "Non-US Pound", false},
    {0x64, -1, "Non-US Backslash", false},

    // Shifted Symbol Keys
    {0x1E, '!', "Exclamation", true}, {0x1F, '@', "At Sign", true}, {0x20, '#', "Hash", true},
    {0x21, '$', "Dollar", true}, {0x22, '%', "Percent", true}, {0x23, '^', "Caret", true},
    {0x24, '&', "Ampersand", true}, {0x25, '*', "Asterisk", true}, {0x26, '(', "Left Parenthesis", true},
    {0x27, ')', "Right Parenthesis", true},

    // Add quote characters
    {0x34, '\'', "Single Quote", false},  // Single quote/apostrophe
    {0x34, '"', "Double Quote", true},     // Double quote (shifted single quote)

    // Navigation Keys
    {0x2C, ' ', "Space", false}, {0x28, '\n', "Enter", false}, {0x29, 27, "Escape", false},
    {0x2A, 8, "Backspace", false}, {0x2B, '\t', "Tab", false}, {0x4C, -1, "Delete", false},
    {0x52, -1, "Up Arrow", false}, {0x51, -1, "Down Arrow", false}, {0x50, -1, "Left Arrow", false},
    {0x4F, -1, "Right Arrow", false}, {0x4A, -1, "Home", false}, {0x4B, -1, "Page Up", false},
    {0x4D, -1, "End", false}, {0x4E, -1, "Page Down", false}, {0x49, -1, "Insert", false},

    // Function Keys
    {0x3A, -1, "F1", false}, {0x3B, -1, "F2", false}, {0x3C, -1, "F3", false}, {0x3D, -1, "F4", false},
    {0x3E, -1, "F5", false}, {0x3F, -1, "F6", false}, {0x40, -1, "F7", false}, {0x41, -1, "F8", false},
    {0x42, -1, "F9", false}, {0x43, -1, "F10", false}, {0x44, -1, "F11", false}, {0x45, -1, "F12", false},
    {0x68, -1, "F13", false}, {0x69, -1, "F14", false}, {0x6A, -1, "F15", false}, {0x6B, -1, "F16", false},
    {0x6C, -1, "F17", false}, {0x6D, -1, "F18", false}, {0x6E, -1, "F19", false}, {0x6F, -1, "F20", false},
    {0x70, -1, "F21", false}, {0x71, -1, "F22", false}, {0x72, -1, "F23", false}, {0x73, -1, "F24", false},

    // System Keys
    {0x46, -1, "Print Screen", false}, {0x65, -1, "Application", false}, {0x66, -1, "Power", false},
    {0x74, -1, "Execute", false}, {0x75, -1, "Help", false}, {0x76, -1, "Menu", false},
    {0x77, -1, "Select", false}, {0x7A, -1, "Undo", false}, {0x7B, -1, "Cut", false},
    {0x7C, -1, "Copy", false}, {0x7D, -1, "Paste", false}, {0x7E, -1, "Find", false},

    // Error Codes
    {0x01, -1, "Error Roll Over", false}, {0x02, -1, "POST Fail", false},
    {0x03, -1, "Error Undefined", false}
};

// Number of entries in the unified key map
constexpr size_t unifiedKeyMapSize = sizeof(unifiedKeyMap) / sizeof(KeyInfo);

#endif // MAGIC_KEYBOARD_KEYMAP_H