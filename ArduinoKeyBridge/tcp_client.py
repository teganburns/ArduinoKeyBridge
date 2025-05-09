#!/usr/bin/env python3
import socket
import threading
import time
import os
from datetime import datetime
import csv
import subprocess
import re

def get_gateway_mac():
    try:
        # Get gateway IP
        route_output = subprocess.check_output(["route", "-n", "get", "default"]).decode()
        gateway_match = re.search(r"gateway: ([\d\.]+)", route_output)
        if not gateway_match:
            print("Could not determine gateway IP")
            return None, None
            
        gateway_ip = gateway_match.group(1)
        print(f"Found gateway IP: {gateway_ip}")
        
        # Get MAC address for gateway IP from ARP table
        arp_output = subprocess.check_output(["arp", "-a"]).decode()
        # Look for the gateway IP in ARP table
        ip_pattern = re.escape(gateway_ip)
        match = re.search(f"{ip_pattern}.*?at ([0-9a-f:]+)", arp_output.lower())
        if match:
            mac = match.group(1)
            print(f"Found gateway MAC: {mac}")
            return gateway_ip, mac
        print(f"No MAC address found for gateway IP {gateway_ip}")
        return gateway_ip, None
    except Exception as e:
        print(f"Error getting gateway info: {e}")
        return None, None

def find_arduino_ip():
    try:
        # Get ARP table
        arp_output = subprocess.check_output(["arp", "-a"]).decode()
        print("ARP Table:")
        print(arp_output)
        
        # Look for our Arduino's MAC address
        mac_pattern = re.escape(ARDUINO_MAC.lower())
        # Updated pattern to match "? (IP) at MAC_ADDRESS" format
        match = re.search(f"\? \(([\d\.]+)\).*?at {mac_pattern}", arp_output.lower())
        if match:
            ip = match.group(1)
            print(f"\nFound Arduino at IP: {ip} (MAC: {ARDUINO_MAC})")
            return ip
            
        print(f"\nNo device found with MAC address {ARDUINO_MAC}")
        return None
    except Exception as e:
        print(f"Error scanning network: {e}")
        return None

# Usage:
os.system('cls' if os.name == 'nt' else 'clear')
print("\U0001F916  ArduinoKeyBridge TCP Client")
print("==========================")
print("This script will connect to the ArduinoKeyBridge device.")
print("Make sure you are connected to the 'ArduinoKeyBridge' WiFi network.\n")

# Use the known Arduino MAC address
ARDUINO_MAC = "f2:f5:bd:4f:87:28"  # Arduino Uno R4 WiFi MAC address

# Find Arduino IP
ARDUINO_IP = find_arduino_ip()
if not ARDUINO_IP:
    print("\n\U0001F6AB  Could not find Arduino on the network. Please ensure you are connected to the ArduinoKeyBridge WiFi network.")
    exit(1)

print(f"\nUsing Arduino IP: {ARDUINO_IP}\n")

ARDUINO_PORT = 8080

connected = False
sock = None

send_lock = threading.Lock()
send_times = {}  # message -> send_time

def log_info(msg):
    print(f"\U0001F4AC  {msg}")

def log_success(msg):
    print(f"\U00002705  {msg}")

def log_error(msg):
    print(f"\U0001F6AB  {msg}")

def log_step(msg):
    print(f"\U0001F449  {msg}")

def now_str():
    return datetime.now().strftime("%H:%M:%S.%f")[:-3]

def connect_to_arduino_tcp():
    global connected, sock
    log_step(f"Connecting to Arduino at {ARDUINO_IP}:{ARDUINO_PORT}...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((ARDUINO_IP, ARDUINO_PORT))
        connected = True
        log_success("Connected to Arduino TCP server!")
        # Send a single null character after connecting
        sock.sendall("CONNECTED".encode('utf-8'))
        log_info("Sent \"CONNECTED\" to Arduino after connecting.")
        return True
    except socket.error as e:
        log_error(f"TCP connection failed: {e}")
        connected = False
        return False

def receive_data():
    global connected, sock
    while connected:
        try:
            data = sock.recv(1024)
            if data:
                if len(data) == 8:
                    print(f"\n--- Raw TCP Packet (8 bytes) ---")
                    print(f"Bytes: {data}")
                    print(f"Hex:   {data.hex(' ')}")
                    print(f"Length: {len(data)} bytes\n")
                    send_key_report(data)  # Echo the packet back
                else:
                    print(f"Ignored packet of length {len(data)} bytes (expected 8 bytes).")
            else:
                log_error("Connection closed by Arduino")
                connected = False
                break
        except socket.error as e:
            log_error(f"Error receiving data: {e}")
            connected = False
            break

def send_key_report(key_report):
    global connected, sock, send_times
    if not isinstance(key_report, bytes) or len(key_report) != 8:
        log_error("send_key_report: key_report must be 8 bytes")
        return
    try:
        with send_lock:
            send_times[key_report] = time.time()
        sock.sendall(key_report)
        log_info(f"Sent key report: {key_report.hex(' ')} at {now_str()}")
    except socket.error as e:
        log_error(f"Error sending data: {e}")
        connected = False

def periodic_key_report_sender():
    global connected
    key_report = bytes([0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00])  # Example: 'a'
    while connected:
        send_key_report(key_report)
        time.sleep(1)

def main():
    global connected
    if not connect_to_arduino_tcp():
        log_error("Failed to connect to Arduino TCP server. Exiting...")
        return
    log_info("Listening for key reports from Arduino...")
    receive_thread = threading.Thread(target=receive_data)
    receive_thread.daemon = True
    receive_thread.start()
    # Start the periodic sender thread
    # periodic_sender_thread = threading.Thread(target=periodic_key_report_sender)
    # periodic_sender_thread.daemon = True
    # periodic_sender_thread.start()
    try:
        while connected:
            time.sleep(1)  # Keep the main thread alive
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        if sock:
            sock.close()

if __name__ == "__main__":
    main() 