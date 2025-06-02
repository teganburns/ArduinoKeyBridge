# commands.py

Implements the `CommandHandler` class used by the server to react to key sequences. Provides macros for screenshots, ChatGPT integration and text-to-speech.

## Key Methods

- `add_report(report)` – Entry point for each received `KeyReport`.
- `toggle_command_mode(report)` – Detect special reports for entering command mode.
- `cmd_screenshot()` – Capture and store a screenshot from the capture card.
- `cmd_archive_screenshot()` / `cmd_delete_screenshot()` – Manage screenshot files and database entries.
- `cmd_toggle_bounding_box()` – Run YOLO detection to choose a crop region.
- `cmd_set_prompt(prompt)` / `cmd_clear_prompt(type)` – Manage the prompt used for ChatGPT requests.
- `cmd_send_to_chatgpt()` – Send active screenshots (or just the prompt) to ChatGPT and store the response.
- `cmd_type_response()` – Type out the last ChatGPT response via the Arduino.
- `cmd_speak()` – Convert the last ChatGPT response to speech and play it.
