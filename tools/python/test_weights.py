from ultralytics import YOLO
import cv2
from collections import deque, Counter

# --- Configuration ---
MODEL_PATH = "weights.pt"  # Path to your downloaded model
DEVICE_INDEX = 0        # Update this if your capture card is not index 0
CONF_THRESHOLD = 0.3    # Minimum confidence to accept detection
CLASS_NAMES = ["default", "no-select", "pointer", "text"]  # Your actual class names
SMOOTHING_FRAMES = 10   # Number of frames for label smoothing

# --- Load model ---
model = YOLO(MODEL_PATH)

# --- Set up video input ---
cap = cv2.VideoCapture(DEVICE_INDEX)
if not cap.isOpened():
    raise RuntimeError("Could not open capture device.")

# --- Buffer for smoothing classification ---
recent_labels = deque(maxlen=SMOOTHING_FRAMES)

print("Running cursor detection. Press 'q' to quit.\n")

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    results = model(frame, verbose=False)[0]

    if results.boxes:
        boxes = results.boxes
        scores = boxes.conf
        labels = boxes.cls

        best_idx = scores.argmax()
        if scores[best_idx] >= CONF_THRESHOLD:
            box = boxes[best_idx]
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            cx, cy = (x1 + x2) // 2, (y1 + y2) // 2
            class_id = int(labels[best_idx])
            conf = float(scores[best_idx])
            class_name = CLASS_NAMES[class_id]

            recent_labels.append(class_id)
            smoothed_class = Counter(recent_labels).most_common(1)[0][0]
            smoothed_name = CLASS_NAMES[smoothed_class]

            # Draw box and label
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            label = f"{smoothed_name} ({conf:.2f})"
            cv2.putText(frame, label, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

            # Print info
            print(f"Detected: {smoothed_name} at ({cx}, {cy}) — Confidence: {conf:.2f}")
    
    # Show the result
    cv2.imshow("Cursor Detection", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()