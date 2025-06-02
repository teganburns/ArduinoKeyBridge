# TCPConnection (TCPConnection.h / TCPConnection.cpp)

Handles the Wi‑Fi access point and TCP networking used to communicate with the Python server. Supports command mode and charter mode for different interaction styles.

## Key Methods

- `startAP()` – Start the Wi‑Fi access point and listen on port 8080.
- `poll()` – Accept a client, read incoming key reports and send them to `MinimalKeyboard`.
- `sendKeyReport(report)` / `sendEmptyKeyReport()` – Send HID reports to the connected client.
- `change_mode(report)` – Detect special key sequences to toggle command or charter mode.
- `type_charter(str)` – Type a string remotely by converting characters to key reports.
- `toggleCharterMode()` – Manually enable or disable charter mode.

The class stores the client connection and manages LED feedback via `ArduinoKeyBridgeNeoPixel`.
