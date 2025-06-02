# ArduinoKeyBridge Server

The Python server communicates with the Arduino firmware over TCP. It receives key reports, processes them, and can issue commands back to the device.

## Main Scripts

| File | Purpose |
| ---- | ------- |
| `main.py` | Starts the TCP client and dispatches incoming key reports to the command handler. |
| `server.py` | Implements the `KeyBridgeTCPServer` class for managing the socket connection. |
| `commands.py` | High level actions triggered by key sequences (e.g. screenshots). |
| `chatgpt_client.py` | Helper for sending screenshots or text to the ChatGPT API. |
| `database.py` | Simple SQLite/MongoDB helpers for storing data. |
| `network_utils.py` | Utilities for discovering the Arduino IP address on the local network. |
| `key_report.py` | Utility class representing an 8‑byte HID key report. |
| `keymap.py` | Mapping table used by `key_report.py` to convert characters to HID codes. |

Additional files such as `config.py` define project‑wide settings. The server relies on the `requirements.txt` file for Python dependencies.
For detailed documentation of each module see [docs/server/README.md](../docs/server/README.md).


To run the server locally:

```bash
pip install -r requirements.txt
python main.py
```
