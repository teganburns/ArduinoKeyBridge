#!/usr/bin/env python3

# Standard library imports
import csv
import os
import re
import socket
import subprocess
import time
from datetime import datetime

# Local application imports
from config import ARDUINO_HOST, ARDUINO_PORT, ARDUINO_MAC
from key_report import KeyReport
from keymap import KEY_MAP
from log import get_logger
from server import KeyBridgeTCPServer
from network_utils import get_gateway_mac, find_arduino_ip, print_startup_message, get_arduino_ip_or_exit

logger = get_logger(__name__)

# Print startup message and find Arduino IP
print_startup_message()
ARDUINO_IP = get_arduino_ip_or_exit()

def main():
    server = KeyBridgeTCPServer(ARDUINO_IP, ARDUINO_PORT)
    server.begin()
    logger.info("Listening for key reports from Arduino...")
    try:
        while server.connected:
            server.poll()
            while server.has_new_report():
                report = server.get_next_report()
                if report.modifiers == 0 and all(k == 0 for k in report.keys):
                    logger.info("Received empty key report, sending '.'")
                    dot_key_report = KeyReport.char_to_key_report('.')
                    server.send_key_report(dot_key_report.to_bytes())
            time.sleep(0.01)  # Prevent busy loop
    except KeyboardInterrupt:
        logger.info("Exiting...")
    finally:
        server.close()

if __name__ == "__main__":
    main() 