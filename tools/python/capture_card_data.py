import cv2
import time
import os

# Directory to save screenshots
output_dir = "capture_card_screenshots"
os.makedirs(output_dir, exist_ok=True)

cap = cv2.VideoCapture(0)  # Replace 0 with your capture card index

if not cap.isOpened():
    raise Exception("Could not open video device")

print("Starting capture. Press Ctrl+C to stop.")
count = 0
while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to capture frame")
        continue

    filename = os.path.join(output_dir, f"screenshot_{time.strftime('%Y%m%d_%H%M%S')}_{count:04}.png")
    cv2.imwrite(filename, frame)
    print(f"Saved: {filename}")
    count += 1
    time.sleep(1)  # Capture every second