# ArduinoKeyBridge.ino

Entry point for the firmware. Sets up logging, the NeoPixel status LED, Wi‑Fi access point and USB host keyboard support. Runs the main loop which processes USB events, handles incoming TCP packets and updates LED animations.

## Key Functions

- `setup()` – Initializes all subsystems and shows progress on the NeoPixels.
- `loop()` – Main loop that polls USB tasks, processes TCP traffic and forwards key reports.
- `handle_new_key_report()` – Processes a new HID report from the connected keyboard and decides whether to send it to the host computer or over the network.

The sketch relies on singleton helpers defined in other modules like `TCPConnection` and `ArduinoKeyBridgeNeoPixel`.
