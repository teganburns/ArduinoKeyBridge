from config import ARDUINO_MAC
from log import get_logger
import subprocess
import re
import os

logger = get_logger(__name__)

def get_gateway_mac():
    """
    Get the gateway IP and its MAC address from the system routing and ARP tables.
    Returns (gateway_ip, mac) or (None, None) on failure.
    """
    try:
        # Get gateway IP
        route_output = subprocess.check_output(["route", "-n", "get", "default"]).decode()
        gateway_match = re.search(r"gateway: ([\d\.]+)", route_output)
        if not gateway_match:
            logger.error("Could not determine gateway IP")
            return None, None
        gateway_ip = gateway_match.group(1)
        logger.info("Found gateway IP: %s", gateway_ip)
        # Get MAC address for gateway IP from ARP table
        arp_output = subprocess.check_output(["arp", "-a"]).decode()
        ip_pattern = re.escape(gateway_ip)
        match = re.search(f"{ip_pattern}.*?at ([0-9a-f:]+)", arp_output.lower())
        if match:
            mac = match.group(1)
            logger.info("Found gateway MAC: %s", mac)
            return gateway_ip, mac
        logger.warning("No MAC address found for gateway IP %s", gateway_ip)
        return gateway_ip, None
    except Exception as e:
        logger.error("Error getting gateway info: %s", e)
        return None, None

def find_arduino_ip():
    """
    Scan the ARP table for the Arduino's MAC address and return its IP, or None if not found.
    """
    try:
        arp_output = subprocess.check_output(["arp", "-a"]).decode()
        logger.info("ARP Table:\n%s", arp_output)
        mac_pattern = re.escape(ARDUINO_MAC.lower())
        match = re.search(f"\? \\(([\d\.]+)\\).*?at {mac_pattern}", arp_output.lower())
        if match:
            ip = match.group(1)
            logger.info("Found Arduino at IP: %s (MAC: %s)", ip, ARDUINO_MAC)
            return ip
        logger.warning("No device found with MAC address %s", ARDUINO_MAC)
        return None
    except Exception as e:
        logger.error("Error scanning network: %s", e)
        return None

def print_startup_message():
    """
    Clear the screen and print the startup message for the ArduinoKeyBridge TCP Client.
    """
    os.system('cls' if os.name == 'nt' else 'clear')
    logger.info(
        "\U0001F916  ArduinoKeyBridge TCP Client\n"
        "==========================\n"
        "This script will connect to the ArduinoKeyBridge device.\n"
        "Make sure you are connected to the 'ArduinoKeyBridge' WiFi network.\n"
    )

def get_arduino_ip_or_exit():
    """
    Find the Arduino IP using find_arduino_ip(). Log and exit if not found. Return the IP if found.
    """
    arduino_ip = find_arduino_ip()
    if not arduino_ip:
        logger.error(
            "\U0001F6AB  Could not find Arduino on the network. "
            "Please ensure you are connected to the ArduinoKeyBridge WiFi network."
        )
        exit(1)
    logger.info("Using Arduino IP: %s", arduino_ip)
    return arduino_ip 