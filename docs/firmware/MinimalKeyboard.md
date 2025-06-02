# MinimalKeyboard (MinimalKeyboard.h / MinimalKeyboard.cpp)

Implements a lightweight HID keyboard parser. The class captures raw reports from the USB host shield and stores them in a `KeyReport` structure for processing.

## Classes

- `MinimalKeyboard` – Singleton providing `begin()`, `sendReport()`, `onNewKeyReport()` and tracking the latest report.
- `MinimalKeyboardParser` – Derived from `KeyboardReportParser`; forwards parsed reports to `MinimalKeyboard`.

## Usage

`ArduinoKeyBridge.ino` registers `MinimalKeyboardParser` with the USB host library. When new key data is available, `onNewKeyReport()` creates a `KeyReport` and sets `hasNewReport` so the main loop can act on it.
