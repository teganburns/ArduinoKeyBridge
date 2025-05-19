#!/usr/bin/env python3

# Standard library imports
import csv
import os
import re
import socket
import subprocess
import time
from datetime import datetime
from collections import deque

# Local application imports
from config import *
from key_report import KeyReport
from keymap import KEY_MAP
from log import get_logger
from server import KeyBridgeTCPServer
from network_utils import get_gateway_mac, find_arduino_ip, print_startup_message, get_arduino_ip_or_exit
from commands import CommandHandler


logger = get_logger(__name__)

# Print startup message and find Arduino IP
print_startup_message()
ARDUINO_IP = get_arduino_ip_or_exit()

def send_dot_on_empty_key_report(server, report):
    """
    If the given KeyReport is empty, send a '.' key report using the server.
    """
    if report.modifiers == 0 and all(k == 0 for k in report.keys):
        logger.info("Received empty key report, sending '.'")
        dot_key_report = KeyReport.char_to_key_report('.')
        server.send_key_report(dot_key_report.to_bytes())

def test_send_key_report(server):
    """
    Test sending a key report to the server.
    """

    # Send the test string
    test_string = (
        "#!/usr/bin/env python3\n"
        "\"\"\"Comprehensive test string for ArduinoKeyBridge.\"\"\"\n"
        "import sys\n"
        "def main():\n"
        "    print('Hello, World!')\n"
        "    print(\"Special chars: !@#$%^&*()_+-=[]{}`~;:'\\\",.<>/?|\\\\\")\n"
        "    print(f\"Numbers: {' '.join(str(i) for i in range(10))}\")\n"
        "    for c in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ':\n"
        "        print(c, end=' ')\n"
        "    print('\\nTabs\\tand\\tspaces')\n"
        "    # End of test\n"
        "\n"
        "if __name__ == '__main__':\n"
        "    main()\n"
    )


    time.sleep(3)
    server.send_string(test_string)
    time.sleep(0.1)

def main():
    server = KeyBridgeTCPServer(ARDUINO_IP, ARDUINO_PORT)
    handler = CommandHandler(server)
    server.begin()
    logger.info("Listening for key reports from Arduino...")

    #test_send_key_report(server)
    try:
        while server.connected:
            server.poll()
            while server.has_new_report():
                report = server.get_next_report()
                if report is not None and not report.is_empty():
                    handler.add_report(report)

            time.sleep(0.01)  # Prevent busy loop
    except KeyboardInterrupt:
        logger.info("Exiting...")
    finally:
        server.close()

if __name__ == "__main__":
    main() 