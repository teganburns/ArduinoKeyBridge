#!/usr/bin/env python3

import cv2
import time
import sys

def list_capture_devices():
    """List all available capture devices."""
    print("Available capture devices:")
    for i in range(10):  # Check first 10 indices
        cap = cv2.VideoCapture(i)
        if cap.isOpened():
            ret, frame = cap.read()
            if ret:
                print(f"Device {i}: Available")
            cap.release()
        else:
            print(f"Device {i}: Not available")

def view_capture_device(device_index=0):
    """View the capture device feed."""
    cap = cv2.VideoCapture(device_index)
    
    if not cap.isOpened():
        print(f"Error: Could not open capture device {device_index}")
        return
    
    print(f"Viewing capture device {device_index}")
    print("Press 'q' to quit")
    
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame")
            break
            
        # Display the frame
        cv2.imshow('Capture Card Feed', frame)
        
        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "--list":
        list_capture_devices()
    else:
        device_index = 0
        if len(sys.argv) > 1:
            try:
                device_index = int(sys.argv[1])
            except ValueError:
                print("Error: Device index must be a number")
                return
        view_capture_device(device_index)

if __name__ == "__main__":
    main() 