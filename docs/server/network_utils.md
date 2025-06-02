# network_utils.py

Helper functions for discovering the Arduino on the network and displaying a startup message.

- `get_gateway_mac()` – Return the gateway IP and MAC address.
- `find_arduino_ip()` – Scan the ARP table for the configured Arduino MAC.
- `get_arduino_ip_or_exit()` – Exit the program if the Arduino cannot be found.
