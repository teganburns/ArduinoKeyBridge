import cv2
from ultralytics import YOLO
import time
import logging
from log import get_logger

logger = get_logger(__name__)

class BoundingBoxManager:
    _box = None  # [x1, y1, x2, y2]
    _model_path = "weights.pt"
    _class_names = ["default", "text", "no-select", "other"]
    _device_index = 0
    _model = YOLO(_model_path)

    @classmethod
    def run_bounding_box_selection(cls, duration=10):
        """
        Run bounding box selection for a fixed duration (in seconds), updating the box with the highest-confidence detection.
        Returns the final box. No display or drawing.
        """
        cls._box = None
        cap = cv2.VideoCapture(cls._device_index)
        start_time = time.time()
        while time.time() - start_time < duration:
            ret, frame = cap.read()
            if not ret:
                continue
            cls.process_frame(frame)
        cap.release()
        if not cls.validate_box():
            logger.error("Bounding box area is too small. Resetting box to [0,0,0,0].")
        return cls.get_box()

    @classmethod
    def process_frame(cls, frame):
        """Helper function to run detection and update the bounding box with the highest-confidence 'default' detection."""
        if cls._model is None:
            return
        results = cls._model(frame, verbose=False)[0]
        best_conf = -1
        best_box = None
        if results.boxes:
            boxes = results.boxes
            scores = boxes.conf
            labels = boxes.cls
            for i in range(len(boxes)):
                class_id = int(labels[i])
                class_name = cls._class_names[class_id]
                conf = float(scores[i])
                if class_name == "default" and conf > best_conf:
                    best_conf = conf
                    x1, y1, x2, y2 = map(int, boxes.xyxy[i])
                    best_box = (x1, y1, x2, y2)
        if best_box:
            x1, y1, x2, y2 = best_box
            cls.update(x1, y1)
            cls.update(x2, y2)

    @classmethod
    def update(cls, x, y):
        if cls._box is None or cls._box == [0, 0, 0, 0]:
            cls._box = [x, y, x, y]
        else:
            cls._box[0] = min(cls._box[0], x)
            cls._box[1] = min(cls._box[1], y)
            cls._box[2] = max(cls._box[2], x)
            cls._box[3] = max(cls._box[3], y)
        logger.info(f"Updated bounding box to {cls._box}")

    @classmethod
    def get_box(cls):
        return tuple(cls._box) if cls._box else None

    @classmethod
    def reset(cls):
        cls._box = None

    @classmethod
    def capture_and_crop(cls):
        """
        Capture a frame from the video device and crop it to the current bounding box.
        Return the cropped image (or None).
        """
        cap = cv2.VideoCapture(cls._device_index)
        ret, frame = cap.read()
        cap.release()
        if not ret or frame is None or frame.size == 0:
            return None
        box = cls.get_box()
        if not box or box == (0, 0, 0, 0):
            return None
        x1, y1, x2, y2 = box
        # Ensure coordinates are within frame bounds
        h, w = frame.shape[:2]
        x1, y1 = max(0, x1), max(0, y1)
        x2, y2 = min(w, x2), min(h, y2)
        cropped = frame[y1:y2, x1:x2]
        if cropped.size > 0:
            return cropped
        return None

    @classmethod
    def validate_box(cls, min_area=1920):
        """
        Validate that the current bounding box area is greater than min_area.
        If not, reset the box to [0,0,0,0] and return False. Otherwise, return True.
        """
        box = cls.get_box()
        if not box or box == (0, 0, 0, 0):
            return False
        x1, y1, x2, y2 = box
        width = max(0, x2 - x1)
        height = max(0, y2 - y1)
        area = width * height
        if area <= min_area:
            cls._box = [0, 0, 0, 0]
            logger.error(f"Bounding box area is too small: {area}. Resetting box to [0,0,0,0].")
            return False
        else:
            logger.info(f"Bounding box area is valid: {area}.")
        return True
