# ArduinoKeyBridge Firmware

This directory contains the main firmware for the **ArduinoKeyBridge** project. The code targets the Arduino UNO R4 WiFi and turns it into a USB keyboard host that can forward key reports over Wi‑Fi and provide visual feedback using NeoPixels.

## File Overview

| File | Description |
| ---- | ----------- |
| `ArduinoKeyBridge.ino` | Entry point that initializes logging, Wi‑Fi access point, USB host support and runs the main loop. |
| `MinimalKeyboard.h` / `MinimalKeyboard.cpp` | Lightweight USB host keyboard parser that converts HID reports into a simple `KeyReport` structure. |
| `MagicKeyboardKeyMap.h` | Lookup table mapping HID key codes to ASCII characters and descriptions. |
| `TCPConnection.h` / `TCPConnection.cpp` | Manages the Wi‑Fi access point and TCP server used to send and receive `KeyReport` packets. Includes "command" and "charter" modes for remote control. |
| `ArduinoKeyBridgeNeoPixel.h` / `ArduinoKeyBridgeNeoPixel.cpp` | Wrapper around `Adafruit_NeoPixel` for status LEDs and setup progress animations. |
| `ArduinoKeyBridgeLogger.h` / `ArduinoKeyBridgeLogger.cpp` | Simple serial logger with different log levels and memory utilities. |
For detailed descriptions of each file and its functions, see [docs/firmware/README.md](../docs/firmware/README.md).


The firmware expects a USB keyboard connected via a USB host shield. It exposes a Wi‑Fi access point named `ArduinoKeyBridge` and listens for incoming connections on port `8080`.

### Building and Uploading

Use the `upload_monitor.sh` script in `tools/bash` to compile and upload the sketch. The script detects bootloader mode and performs a clean build automatically.

```bash
./tools/bash/upload_monitor.sh
```

### Charter Mode

Charter mode lets a remote client send text which is buffered on the Arduino and typed out key by key. Toggle the mode by pressing the `F19` key or by sending the special key report defined in `TCPConnection::change_mode()`.

