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
from bounding_box import BoundingBoxManager
import datetime
from chatgpt_client import ChatGPTClient
import json
import uuid

logger = get_logger(__name__)
database = dbManager()

class CommandHandler:
    """
    Command handling logic and character buffer for ArduinoKeyBridge.
    Extend this class to implement macros or device control.
    """
    def __init__(self, server):
        self._buffer = deque()
        self.command_map = {
            "screenshot": self.cmd_screenshot,
            "archive_screenshot": self.cmd_archive_screenshot,
            "delete_screenshot": self.cmd_delete_screenshot,
            "list_screenshots": self.cmd_list_screenshot,
            "list_active_screenshots": self.cmd_list_active_screenshot,
            "list_archived_screenshots": self.cmd_list_archive_screenshot,
            "toggle_bounding_box": self.cmd_toggle_bounding_box,
            "set_prompt": self.cmd_set_prompt,
            "clear_prompt": self.cmd_clear_prompt,
            "send_to_chatgpt": self.cmd_send_to_chatgpt,
            "type_response": self.cmd_type_response
        }
        # Add this alias map
        self.alias_map = {
            "F13": "screenshot",  # You can use key names or keycodes
            "F16": "toggle_bounding_box",  # Add F16 alias
            "F1": "set_prompt",  # F1 triggers set_prompt
            "F2": "clear_prompt",  # F2 triggers clear_prompt (code)
            "F3": "clear_prompt",  # F3 triggers clear_prompt (multiple choice)
            # Add more aliases as needed
        }
        self.server = server
        self.logger = get_logger(__name__)
        self._chatgpt_prompt = None

        # Ensure screenshot output directories exist
        for d in (SCREENSHOT_ACTIVE_DIR, SCREENSHOT_ARCHIVE_DIR):
            if d and not os.path.exists(d):
                os.makedirs(d, exist_ok=True)

        self._running = False

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

        # Check for special key alias (e.g., F13)
        for key in report.keys:
            if key == 0x68:  # F13
                alias_command = self.alias_map.get("F13")
                if alias_command:
                    self.logger.info(f"Alias detected: F13 -> {alias_command}")
                    self.handle_command(alias_command)
                return
            if key == 0x6B:  # F16 (107 decimal)
                alias_command = self.alias_map.get("F16")
                if alias_command:
                    self.logger.info(f"Alias detected: F16 -> {alias_command}")
                    self.handle_command(alias_command)
                return
            if key == 0x3A:  # F1 (58 decimal)
                alias_command = self.alias_map.get("F1")
                if alias_command:
                    self.logger.info(f"Alias detected: F1 -> {alias_command}")
                    self.handle_command(alias_command)
                return
            if key == 0x3B:  # F2 (59 decimal)
                alias_command = self.alias_map.get("F2")
                if alias_command:
                    self.logger.info(f"Alias detected: F2 -> {alias_command} (code)")
                    self.cmd_clear_prompt(2)
                return
            if key == 0x3C:  # F3 (60 decimal)
                alias_command = self.alias_map.get("F3")
                if alias_command:
                    self.logger.info(f"Alias detected: F3 -> {alias_command} (multiple choice)")
                    self.cmd_clear_prompt(3)
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
        if key_set == {0x10}:
            self.on()
            self.logger.info("Command mode ON triggered by special report.")
            return True
        elif key_set == {0x11}:
            self.off()
            self.logger.info("Command mode OFF triggered by special report.")
            return True
        elif key_set == {0x12}:
            # Good command
            self.logger.info("Special command (all keys 12) received - good command.")
            return True
        elif key_set == {0x13}:
            # Bad command
            self.logger.info("Special command (all keys 13) received - bad command.")
            return True
        else:
            self.logger.info(f"Unknown special command received. Modifiers: {hex(report.modifiers)}, Keys: {[hex(k) for k in report.keys]}")
            return False

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
            error_report = KeyReport(modifiers=0x22, keys=[13]*6)
            self.server.send_key_report(error_report.to_bytes())
            return None
        self.clear_buffer()

    def is_command(self):
        """
        Is the current buffer a complete command?
        """
        return self.buffer() in self.command_map
    
    def send_success_report(self):
        """
        Send a success report to the Arduino.
        """
        success_report = KeyReport(modifiers=0x22, keys=[12]*6)
        for i in range(STATUS_RETRY_COUNT):
            self.server.send_key_report(success_report.to_bytes())

    def send_error_report(self):
        """
        Send an error report to the Arduino.
        """
        error_report = KeyReport(modifiers=0x22, keys=[13]*6)
        for i in range(STATUS_RETRY_COUNT):
            self.server.send_key_report(error_report.to_bytes())

    def send_charter_mode_report(self):
        """
        Send a charter mode report to the Arduino.
        """
        charter_report = KeyReport(modifiers=0x22, keys=[2]*6)
        for i in range(STATUS_RETRY_COUNT):
            self.server.send_key_report(charter_report.to_bytes())
    
    def send_pending_report(self):
        """
        Send a pending report to the Arduino.
        """
        pending_report = KeyReport(modifiers=0x22, keys=[14]*6)
        for i in range(STATUS_RETRY_COUNT):
            self.server.send_key_report(pending_report.to_bytes())

    #####################################
    # Command Handler Methods
    # All command-specific handler methods
    # are organized at bottom of class
    #####################################

    def cmd_screenshot(self, device_index=0, output_path=None):
        """
        Handle the 'screenshot' command. Capture a frame from the capture card and save it in the active screenshots directory with a timestamped filename.
        If a valid bounding box is active, crop to that region before saving.
        """
        logger = get_logger(__name__)
        if output_path is None:
            timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
            output_path = os.path.join(SCREENSHOT_ACTIVE_DIR, f'screenshot_{timestamp}.png')
        logger.info(f"Attempting to capture screenshot from capture card (device {device_index})...")

        box = BoundingBoxManager.get_box()
        if box and box != (0, 0, 0, 0):
            logger.info("Bounding box is active. Capturing and saving cropped screenshot.")
            cropped = BoundingBoxManager.capture_and_crop()
            if cropped is not None and cropped.size > 0:
                cv2.imwrite(output_path, cropped)
                logger.info(f"Cropped screenshot saved to {output_path}")
            else:
                logger.error("Failed to capture or crop screenshot. Falling back to full frame.")
                cap = cv2.VideoCapture(device_index)
                ret, frame = cap.read()
                cap.release()
                if not ret:
                    logger.error("Failed to capture frame from capture card.")
                    return "capture_failed"
                cv2.imwrite(output_path, frame)
                logger.info(f"Full screenshot saved to {output_path}")
        else:
            logger.info("Bounding box is not active. Capturing and saving full screenshot.")
            cap = cv2.VideoCapture(device_index)
            ret, frame = cap.read()
            cap.release()
            if not ret:
                logger.error("Failed to capture frame from capture card.")
                return "capture_failed"
            cv2.imwrite(output_path, frame)
            logger.info(f"Screenshot saved to {output_path}")

        if(database.insert_screenshot(os.path.basename(output_path), SCREENSHOT_ACTIVE_DIR, 'active')):
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

    def cmd_toggle_bounding_box(self, x=0, y=0):
        """
        Run bounding box selection and then crop/display the result.
        """
        logger.info("Running bounding box selection.")
        BoundingBoxManager.run_bounding_box_selection()
        box = BoundingBoxManager.get_box()
        logger.info(f"Bounding box selection complete. Box: {box}")
        if box and box != (0, 0, 0, 0):
            self.send_success_report()
        else:
            self.send_error_report()
            logger.error("Failed to select bounding box.")
            return "bounding_box_error"

    def cmd_set_prompt(self, prompt=None):
        """
        Set a custom prompt for ChatGPT requests. If no prompt is provided, use the buffer.
        Usage: set_prompt <your prompt here> or press F1 after typing prompt.
        Handles status LEDs and buffer clearing.
        """
        if prompt is None or not isinstance(prompt, str) or not prompt.strip():
            # Use buffer if no prompt provided
            prompt = self.buffer().strip()
        if not prompt:
            self.logger.error("No prompt provided. Usage: set_prompt <your prompt here>")
            self.send_error_report()
            self.clear_buffer()
            return 
        self._chatgpt_prompt = prompt
        self.logger.info(f"ChatGPT prompt set to: {self._chatgpt_prompt}")
        self.send_success_report()
        self.clear_buffer()

    def cmd_clear_prompt(self, prompt_type=None):
        """
        Reset the ChatGPT prompt to a predefined prompt based on prompt_type.
        prompt_type: 2 for code, 3 for multiple choice, else default.
        Usage: clear_prompt [prompt_type] or press F2/F3.
        Handles status LEDs and buffer clearing.
        """
        if prompt_type == 2:
            self._chatgpt_prompt = DEFAULT_PROMPT_CODE
            self.logger.info(f"ChatGPT prompt set to code: {DEFAULT_PROMPT_CODE}")
        elif prompt_type == 3:
            self._chatgpt_prompt = DEFAULT_PROMPT_MULTIPLE_CHOICE
            self.logger.info(f"ChatGPT prompt set to multiple choice: {DEFAULT_PROMPT_MULTIPLE_CHOICE}")
        else:
            if self._chatgpt_prompt is None:
                self._chatgpt_prompt = DEFAULT_PROMPT
                self.logger.info(f"ChatGPT prompt reset to default: {DEFAULT_PROMPT}")
            else:
                self._chatgpt_prompt = self.buffer().strip()
                self.logger.info(f"ChatGPT prompt reset to default: {self._chatgpt_prompt}")
        self.send_success_report()
        self.clear_buffer()
        return "prompt_reset_to_default"

    def cmd_send_to_chatgpt(self):
        """
        Send all active screenshots to ChatGPT with the current prompt, or just the prompt if no images exist. Log the response.
        Save the response as a JSON file in the active ChatGPT directory and add an entry to the database with associated screenshot file paths.
        """
        prompt = self._chatgpt_prompt or DEFAULT_PROMPT
        try:
            screenshots = database.list_active_screenshot()
            chatgpt = ChatGPTClient()
            if screenshots:
                image_paths = [os.path.join(s['path'], s['filename']) for s in screenshots]
                self.logger.info(f"Sending {len(image_paths)} images to ChatGPT with prompt: {prompt}")
                response = chatgpt.send_image_with_prompt(image_paths, prompt=prompt)
            else:
                self.logger.info(f"No active screenshots found. Sending prompt only: {prompt}")
                image_paths = []
                response = chatgpt.send_prompt(prompt)
            self.logger.info(f"ChatGPT response: {response}")

            # Save response as JSON file in active ChatGPT directory
            timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
            file_id = str(uuid.uuid4())
            filename = f"chatgpt_{timestamp}_{file_id}.json"
            output_path = os.path.join(CHATGPT_ACTIVE_DIR, filename)
            with open(output_path, 'w') as f:
                json.dump({
                    'response': response,
                    'prompt': prompt,
                    'screenshots': image_paths,
                    'timestamp': timestamp
                }, f, indent=2)
            self.logger.info(f"ChatGPT response saved to {output_path}")

            # Insert entry into the database
            database.chatgpt.insert_one({
                'file_id': file_id,
                'filename': filename,
                'path': CHATGPT_ACTIVE_DIR,
                'timestamp': timestamp,
                'prompt': prompt,
                'response': response,
                'screenshots': image_paths,
                'status': 'active'
            })
            self.logger.info(f"ChatGPT response inserted into database with file_id {file_id}")
        except Exception as e:
            self.logger.error(f"Error sending to ChatGPT: {e}")
            self.send_error_report()
        self.send_success_report()
        return

    def cmd_type_response(self):
        """
        Retrieve the most recent ChatGPT response from the database and type it out using the server.
        """
        try:
            # Get the most recent active ChatGPT response
            doc = database.chatgpt.find_one({"status": "active"}, sort=[("timestamp", -1)])
            if not doc:
                self.logger.error("No active ChatGPT response found in database.")
                self.send_error_report()
                return
            response = doc.get("response")
            # Try to extract the main content from the response
            content = None
            if isinstance(response, dict):
                # OpenAI format: response['choices'][0]['message']['content']
                try:
                    content = response['choices'][0]['message']['content']
                except Exception:
                    content = str(response)
            else:
                content = str(response)
            if not content:
                self.logger.error("No content found in ChatGPT response.")
                self.send_error_report()
                return
            self.logger.info(f"Typing response: {content}")
            self.server.send_string(content)
            self.send_success_report()
        except Exception as e:
            self.logger.error(f"Error typing ChatGPT response: {e}")
            self.send_error_report()
        return