# HID key mapping and report generation based on MagicKeyboardKeyMap.h

KEY_MAP = {
    'a': (0x04, False), 'A': (0x04, True),
    'b': (0x05, False), 'B': (0x05, True),
    'c': (0x06, False), 'C': (0x06, True),
    'd': (0x07, False), 'D': (0x07, True),
    'e': (0x08, False), 'E': (0x08, True),
    'f': (0x09, False), 'F': (0x09, True),
    'g': (0x0A, False), 'G': (0x0A, True),
    'h': (0x0B, False), 'H': (0x0B, True),
    'i': (0x0C, False), 'I': (0x0C, True),
    'j': (0x0D, False), 'J': (0x0D, True),
    'k': (0x0E, False), 'K': (0x0E, True),
    'l': (0x0F, False), 'L': (0x0F, True),
    'm': (0x10, False), 'M': (0x10, True),
    'n': (0x11, False), 'N': (0x11, True),
    'o': (0x12, False), 'O': (0x12, True),
    'p': (0x13, False), 'P': (0x13, True),
    'q': (0x14, False), 'Q': (0x14, True),
    'r': (0x15, False), 'R': (0x15, True),
    's': (0x16, False), 'S': (0x16, True),
    't': (0x17, False), 'T': (0x17, True),
    'u': (0x18, False), 'U': (0x18, True),
    'v': (0x19, False), 'V': (0x19, True),
    'w': (0x1A, False), 'W': (0x1A, True),
    'x': (0x1B, False), 'X': (0x1B, True),
    'y': (0x1C, False), 'Y': (0x1C, True),
    'z': (0x1D, False), 'Z': (0x1D, True),
    '1': (0x1E, False), '!': (0x1E, True),
    '2': (0x1F, False), '@': (0x1F, True),
    '3': (0x20, False), '#': (0x20, True),
    '4': (0x21, False), '$': (0x21, True),
    '5': (0x22, False), '%': (0x22, True),
    '6': (0x23, False), '^': (0x23, True),
    '7': (0x24, False), '&': (0x24, True),
    '8': (0x25, False), '*': (0x25, True),
    '9': (0x26, False), '(': (0x26, True),
    '0': (0x27, False), ')': (0x27, True),
    ' ': (0x2C, False), '\n': (0x28, False),
    '-': (0x2D, False), '_': (0x2D, True),
    '=': (0x2E, False), '+': (0x2E, True),
    '[': (0x2F, False), '{': (0x2F, True),
    ']': (0x30, False), '}': (0x30, True),
    '\\': (0x31, False), '|': (0x31, True),
    ';': (0x33, False), ':': (0x33, True),
    "'": (0x34, False), '"': (0x34, True),
    '`': (0x35, False), '~': (0x35, True),
    ',': (0x36, False), '<': (0x36, True),
    '.': (0x37, False), '>': (0x37, True),
    '/': (0x38, False), '?': (0x38, True),
    '\t': (0x2B, False),
}

def char_to_key_report(char):
    # Handle newline and tab
    if char == '\n':
        keycode, shifted = KEY_MAP.get('\n', (0x00, False))
    elif char == '\t':
        keycode, shifted = KEY_MAP.get('\t', (0x00, False))
    else:
        keycode, shifted = KEY_MAP.get(char, (0x00, False))
    modifier = 0x02 if shifted else 0x00
    # 8-byte HID report: [modifier, 0x00, keycode, 0, 0, 0, 0, 0]
    return bytes([modifier, 0x00, keycode, 0x00, 0x00, 0x00, 0x00, 0x00]) 