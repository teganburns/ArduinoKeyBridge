# bounding_box.py

Manages selection of a screen region using a YOLO model. Provides cropping utilities for screenshots.

## Key Functions

- `run_bounding_box_selection(duration)` – Capture video frames for a short period and store the best detection.
- `process_frame(frame)` – Helper to update the bounding box from YOLO results.
- `update(x, y)` – Grow the stored box to include the point.
- `get_box()` / `reset()` – Access or clear the current box.
- `capture_and_crop()` – Return a cropped frame using the saved box.
- `validate_box(min_area)` – Ensure the box is larger than the given area.
