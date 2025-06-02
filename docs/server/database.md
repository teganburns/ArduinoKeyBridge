# database.py

Wrapper around MongoDB used for storing screenshots and ChatGPT responses. Creates needed directories and provides helper methods for inserting and querying records.

## Key Methods

- `insert_screenshot(filename, path, status)` – Add a screenshot record.
- `get_last_screenshot()` – Retrieve the most recent screenshot entry.
- `archive_screenshot(file_id)` / `delete_screenshot(file_id)` – Move or remove screenshot files and update the database.
- `list_active_screenshot()` / `list_archive_screenshot()` – Convenience listing helpers.
- `reset_database()` – Wipe all collections and delete stored files (used for development).
