import cv2
import time
from ultralytics import YOLO
from collections import deque, Counter

MODEL_PATH = "weights.pt"  # Path to your YOLO model
DEVICE_INDEX = 0
CONF_THRESHOLD = 0.3
CLASS_NAMES = ["default", "text", "no-select", "other"]
SMOOTHING_FRAMES = 10
RESET_INTERVAL = 10  # seconds

model = YOLO(MODEL_PATH)
cap = cv2.VideoCapture(DEVICE_INDEX)
if not cap.isOpened():
    raise RuntimeError("Could not open capture device.")

recent_labels = deque(maxlen=SMOOTHING_FRAMES)
top_left = [float('inf'), float('inf')]
bottom_right = [0, 0]
last_reset = time.time()

print("Running bounding box demo. Press 'q' to quit. Bounding box resets every 5 seconds.\n")

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

            if class_name == "default":
                # Update top-left
                if x1 < top_left[0]:
                    top_left[0] = x1
                if y1 < top_left[1]:
                    top_left[1] = y1
                # Update bottom-right
                if x2 > bottom_right[0]:
                    bottom_right[0] = x2
                if y2 > bottom_right[1]:
                    bottom_right[1] = y2

    # Reset the large bounding box every RESET_INTERVAL seconds
    if time.time() - last_reset >= RESET_INTERVAL:
        print(f"Bounding box reset. Previous box: top_left={tuple(top_left)}, bottom_right={tuple(bottom_right)}")
        top_left = [float('inf'), float('inf')]
        bottom_right = [0, 0]
        last_reset = time.time()

    # Draw the large bounding box if it has been updated
    if top_left[0] < float('inf') and top_left[1] < float('inf') and bottom_right[0] > 0 and bottom_right[1] > 0:
        cv2.rectangle(frame, tuple(top_left), tuple(bottom_right), (0, 0, 255), 2)

    cv2.imshow("Bounding Box Demo", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
