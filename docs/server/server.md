# server.py

Defines the `KeyBridgeTCPServer` class which manages the TCP connection to the Arduino firmware.

## Key Methods

- `connect_and_init()` – Connect to the Arduino and send an initial empty report.
- `send_key_report()` / `send_string()` – Send raw reports or text via charter mode.
- `receive_key_report()` – Read 8-byte reports with timeout handling.
- `poll()` – Non-blocking check for new reports and queue them.
- `begin()` – Loop until a connection succeeds then start polling.
