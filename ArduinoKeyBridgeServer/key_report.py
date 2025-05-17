"""
key_report.py
-------------
KeyReport class for HID report logic and conversions.
"""
from typing import List, Tuple
from keymap import KEY_MAP

class KeyReport:
    def __init__(self, modifiers: int = 0, keys: List[int] = None):
        self.modifiers = modifiers
        self.reserved = 0
        self.keys = keys if keys is not None else [0] * 6

    @classmethod
    def from_tuple(cls, tup: Tuple[int, int, int, int, int, int, int, int]):
        """
        Create a KeyReport from an 8-tuple (modifier, reserved, key1, ..., key6)
        """
        if len(tup) != 8:
            raise ValueError("KeyReport tuple must be 8 elements")
        modifiers = tup[0]
        keys = list(tup[2:])
        return cls(modifiers, keys)

    @classmethod
    def from_bytes(cls, data: bytes):
        """
        Create a KeyReport from 8 bytes.
        """
        if len(data) != 8:
            raise ValueError("KeyReport must be 8 bytes")
        modifiers = data[0]
        keys = list(data[2:8])
        return cls(modifiers, keys)

    def to_bytes(self) -> bytes:
        """
        Serialize the KeyReport to 8 bytes.
        """
        return bytes([self.modifiers, self.reserved] + self.keys)

    @staticmethod
    def char_to_key_report(char: str) -> 'KeyReport':
        """
        Convert a character to a KeyReport object.
        """
        if char == '\n':
            keycode, shifted = KEY_MAP.get('\n', (0x00, False))
        elif char == '\t':
            keycode, shifted = KEY_MAP.get('\t', (0x00, False))
        else:
            keycode, shifted = KEY_MAP.get(char, (0x00, False))
        modifiers = 0x02 if shifted else 0x00
        return KeyReport(modifiers, [keycode] + [0]*5)

    @staticmethod
    def key_report_to_char(modifiers: int, keycode: int) -> str:
        """
        Convert a modifier and keycode back to a character, if possible.
        Returns the character or an empty string if not found.
        """
        for char, (code, shifted) in KEY_MAP.items():
            if code == keycode and ((modifiers & 0x02) != 0) == shifted:
                return char
        return ''

    def __repr__(self):
        return f"<KeyReport modifiers=0x{self.modifiers:02X} keys={self.keys}>" 