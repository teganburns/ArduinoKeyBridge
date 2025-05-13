import asyncio
from bleak import BleakClient, BleakScanner
import struct
import sys

# UUIDs (must match Arduino)
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# Device names to look for
DEVICE_NAMES = ["ArduinoKeyBridge", "Arduino"]

# Connection settings
MAX_RETRIES = 3
RETRY_DELAY = 2  # seconds

async def notification_handler(sender, data):
    """Handle incoming notifications from the Arduino."""
    try:
        # Try to decode as text
        text = data.decode('utf-8')
        print(f"Received text: {text}")
    except UnicodeDecodeError:
        # If not text, show as hex
        print(f"Received data: {data.hex()}")

async def connect_to_device(target_device, retry_count=0):
    """Attempt to connect to the device with retry logic."""
    try:
        async with BleakClient(target_device.address) as client:
            print("Connected!")
            print("Waiting for text from Arduino...")
            
            # Set up notification handler
            await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
            print("Notification handler set up")
            
            # Keep the connection alive
            while True:
                await asyncio.sleep(1)
                
    except Exception as e:
        print(f"Connection error: {str(e)}")
        if retry_count < MAX_RETRIES:
            print(f"Retrying in {RETRY_DELAY} seconds... (Attempt {retry_count + 1}/{MAX_RETRIES})")
            await asyncio.sleep(RETRY_DELAY)
            return await connect_to_device(target_device, retry_count + 1)
        else:
            print("Max retries reached. Exiting...")
            sys.exit(1)

async def main():
    print("Searching for BLE devices...")
    
    # Scan for devices
    devices = await BleakScanner.discover()
    
    # Print all discovered devices
    print("\nDiscovered devices:")
    for device in devices:
        print(f"Device: {device.name or 'Unknown'} - Address: {device.address}")
        if device.name:
            print(f"  Name: {device.name}")
        if device.metadata.get('uuids'):
            print(f"  Services: {device.metadata['uuids']}")
        print("---")
    
    target_device = None
    for device in devices:
        if device.name and any(name in device.name for name in DEVICE_NAMES):
            target_device = device
            break
    
    if not target_device:
        print(f"\nNo Arduino device found! Looking for: {', '.join(DEVICE_NAMES)}")
        return
    
    print(f"\nFound device: {target_device.name}")
    print(f"Address: {target_device.address}")
    
    # Connect to the device with retry logic
    await connect_to_device(target_device)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nExiting...")
        sys.exit(0) 