"""
commands.py
-----------
Command handling logic for ArduinoKeyBridge (e.g., macros, device control).
"""

from collections import deque
from key_report import KeyReport
from log import get_logger
import cv2
from config import *
import os
from server import KeyBridgeTCPServer
import difflib
from database import dbManager

logger = get_logger(__name__)
database = dbManager()

class CommandHandler:
    """
    Command handling logic and character buffer for ArduinoKeyBridge.
    Extend this class to implement macros or device control.
    """
    def __init__(self):
        self._buffer = deque()
        self.command_map = {
            "screenshot": self.cmd_screenshot,
            "archive_screenshot": self.cmd_archive_screenshot,
            "delete_screenshot": self.cmd_delete_screenshot,
            "list_screenshots": self.cmd_list_screenshot,
            "list_active_screenshots": self.cmd_list_active_screenshot,
            "list_archived_screenshots": self.cmd_list_archive_screenshot
        }

        # Ensure screenshot output directories exist
        for d in (SCREENSHOT_ACTIVE_DIR, SCREENSHOT_ARCHIVE_DIR):
            if d and not os.path.exists(d):
                os.makedirs(d, exist_ok=True)
        self.logger = get_logger(__name__)

    def on(self):
        """
        Turn on command mode or enable the handler (placeholder for future logic).
        """
        # Implement enabling logic here
        self.clear_buffer()
        logger.info("Sent special KeyReport to Arduino to trigger blue LED mode.")
        pass

    def off(self):
        """
        Turn off command mode or disable the handler (placeholder for future logic).
        """
        # Implement disabling logic here
        self.clear_buffer()
        logger.info("Sent special KeyReport to Arduino to trigger white LED mode.")
        pass

    def status(self):
        """
        Return the status of the handler (placeholder for future logic).
        """
        logger.info("CommandHandler: STATUS called (implement logic as needed)")
        # Return actual status here
        return False

    def add_report(self, report):
        """
        Process a KeyReport: handle command mode toggling or buffer normal characters.
        """
        if report.is_empty():
            return

        if self.toggle_command_mode(report):
            return  # Special command handled

        char = KeyReport.key_report_to_char(report.modifiers, report.keys[0])
        if char and char == '\n':
            command = self.buffer().strip()
            if command:
                self.handle_command(command)
        else:
            self._buffer.append(char)
 
        self.log_buffer()

    def clear_buffer(self):
        """
        Clear the current buffer.
        """
        self._buffer.clear()

    def log_buffer(self):
        """
        Log the current buffer contents.
        """
        buffer_str = self.buffer()
        if buffer_str:
            self.logger.info("Current buffer contents: %r", buffer_str)
        else:
            self.logger.info("Buffer is empty")

    def toggle_command_mode(self, report):
        """
        Handle command mode toggling and special command reports.
        """
        if report.modifiers != 0x22:
            return False  # Not a command mode report

        key_set = set(report.keys)
        if key_set == {10}:
            self.off()
            self.logger.info("Command mode OFF triggered by special report.")
            return True
        elif key_set == {11}:
            self.on()
            self.logger.info("Command mode ON triggered by special report.")
            return True
        elif key_set == {12}:
            # Good command
            self.logger.info("Special command (all keys 12) received - good command.")
            return True
        elif key_set == {13}:
            # Bad command
            self.logger.info("Special command (all keys 13) received - bad command.")
            return True

        return False  # Not a recognized special command

    def buffer(self):
        """
        Return the current buffer as a string.
        """
        return ''.join(self._buffer)

    def fuzzy_match_command(self, command):
        """
        Return the closest matching command from command_map, or None if no good match.
        """
        close_matches = difflib.get_close_matches(command, self.command_map.keys(), n=1, cutoff=0.4)
        return close_matches[0] if close_matches else None

    def handle_command(self, command):
        handler = self.command_map.get(command)
        if handler:
            return handler()
        else:
            # Try fuzzy matching
            best_match = self.fuzzy_match_command(command)
            if best_match:
                self.logger.info(f"Did you mean '{best_match}'? Executing it.")
                return self.command_map[best_match]()
            self.logger.warning(f"Unknown command: {command}")
            # send message to make leds red (special KeyReport: modifiers=0x22, keys=[13]*6)
            server = KeyBridgeTCPServer.get_instance()
            error_report = KeyReport(modifiers=0x22, keys=[13]*6)
            server.send_key_report(error_report.to_bytes())
            self.clear_buffer()
            return None

    def is_command(self):
        """
        Is the current buffer a complete command?
        """
        return self.buffer() in self.command_map
    
    def send_success_report(self):
        """
        Send a success report to the Arduino.
        """
        server = KeyBridgeTCPServer.get_instance()
        success_report = KeyReport(modifiers=0x22, keys=[12]*6)
        for i in range(STATUS_RETRY_COUNT):
            server.send_key_report(success_report.to_bytes())

    def send_error_report(self):
        """
        Send an error report to the Arduino.
        """
        server = KeyBridgeTCPServer.get_instance()
        error_report = KeyReport(modifiers=0x22, keys=[13]*6)
        for i in range(STATUS_RETRY_COUNT):
            server.send_key_report(error_report.to_bytes())

    #####################################
    # Command Handler Methods
    # All command-specific handler methods
    # are organized at bottom of class
    #####################################

    def cmd_screenshot(self, device_index=0, output_path=None):
        """
        Handle the 'screenshot' command. Capture a frame from the capture card and save it in the active screenshots directory with a timestamped filename.
        """
        import datetime
        from config import SCREENSHOT_ACTIVE_DIR
        logger = get_logger(__name__)
        if output_path is None:
            timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
            output_path = os.path.join(SCREENSHOT_ACTIVE_DIR, f'screenshot_{timestamp}.png')
        logger.info(f"Attempting to capture screenshot from capture card (device {device_index})...")
        cap = cv2.VideoCapture(device_index)
        if not cap.isOpened():
            logger.error(f"Could not open capture card at device index {device_index}")
            return "capture_card_error"
        ret, frame = cap.read()
        cap.release()
        if not ret:
            logger.error("Failed to capture frame from capture card.")
            return "capture_failed"
        cv2.imwrite(output_path, frame)
        logger.info(f"Screenshot saved to {output_path}")

        if(database.insert_screenshot(f'screenshot_{timestamp}.png',SCREENSHOT_ACTIVE_DIR, 'active')):
            self.send_success_report()
            logger.info("Screenshot inserted into database.")
        else:
            self.send_error_report()
            logger.error("Failed to insert screenshot into database.")
            return "database_error"
        
        return output_path


    def cmd_archive_screenshot(self):
        """
        Handle the 'archive' command. Archive the current screenshot.
        """
        # get the last screenshot from the database
        screenshot = database.get_last_screenshot()
        if screenshot:
            database.archive_screenshot(screenshot['file_id'])
            logger.info(f"Archived screenshot from commands.py: {screenshot['file_id']}")
            self.send_success_report()
            logger.info("Screenshot archived.")
        else:
            self.send_error_report()
            logger.error("Failed to archive screenshot.")
            return "database_error"

    def cmd_delete_screenshot(self):
        """
        Handle the 'delete' command. Delete the lst screenshot.
        """
        # get the last screenshot from the database
        screenshot = database.get_last_screenshot()
        if screenshot:
            database.delete_screenshot(screenshot['file_id'])
            self.send_success_report()
            logger.info("Screenshot deleted.")
        else:
            self.send_error_report()
            logger.error("Failed to delete screenshot.")
            return "database_error"

    def cmd_list_screenshot(self):
        """
        Handle the 'list' command. List all screenshot.
        """
        # call list screenshot
        screenshots = database.list_screenshot()
        if screenshots:
            self.send_success_report()
            for screenshot in screenshots:
                logger.info(f"Screenshot: {screenshot}")
        else:
            self.send_error_report()
            logger.info("No screenshots found")

    def cmd_list_active_screenshot(self):
        """
        Handle the 'list' command. List all screenshot.
        """
        screenshots = database.list_active_screenshot()
        if screenshots:
            self.send_success_report()
            for screenshot in screenshots:
                logger.info(f"Active screenshot found: {screenshot}")
        else:
            self.send_error_report()
            logger.info("No active screenshots found")

    def cmd_list_archive_screenshot(self):
        """
        Handle the 'list' command. List all archive screenshot.
        """
        screenshots = database.list_archive_screenshot()
        if screenshots:
            self.send_success_report()
            for screenshot in screenshots:
                logger.info(f"Archive screenshot found: {screenshot}")
        else:
            self.send_error_report()
            logger.info("No archive screenshots found")