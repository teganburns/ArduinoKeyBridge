# key_report.py

Defines the `KeyReport` class which represents an HID key report and provides conversion helpers.

## Key Methods

- `from_tuple()` and `from_bytes()` – Create an instance from raw data.
- `to_bytes()` – Serialize back to 8 bytes.
- `char_to_key_report(char)` – Convert a character to a `KeyReport` using `KEY_MAP`.
- `key_report_to_char(modifiers, keycode)` – Reverse lookup from key code to character.
- `is_empty()` – True if no modifiers and all keys are zero.
