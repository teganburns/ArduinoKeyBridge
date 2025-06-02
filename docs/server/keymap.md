# keymap.py

Mapping table from characters to HID key codes used by the Python server. Mirrors the lookup table in the firmware.

Use `KEY_MAP[char]` to obtain `(keycode, shifted)` pairs for building `KeyReport` instances.
