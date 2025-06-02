# MagicKeyboardKeyMap.h

Lookup table mapping HID key codes to ASCII characters and descriptions. Used by logging and by the server for translating between characters and key codes.

## Contents

Defines `KeyInfo` structures in `unifiedKeyMap` along with `unifiedKeyMapSize`. Each entry records the HID hex code, ASCII value, text description and whether the character requires the Shift modifier.
