import asyncio
from bleak import BleakClient, BleakScanner
import struct
import sys
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# UUIDs (must match Arduino)
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
TX_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"  # Arduino → Central
RX_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a9"  # Central → Arduino

# Connection settings
MAX_RETRIES = 3
RETRY_DELAY = 2  # seconds

class KeyBridgeClient:
    def __init__(self):
        self.client = None
        self.connected = False
        self.connection_event = asyncio.Event()

    async def indication_handler(self, sender, data):
        """Handle incoming indications from the Arduino."""
        try:
            # Try to decode as text
            text = data.decode('utf-8')
            logger.info(f"Received indication: {text}")
        except UnicodeDecodeError:
            # If not text, show as hex
            logger.info(f"Received data: {data.hex()}")

    async def send_data(self, data):
        """Send data to the Arduino and wait for confirmation."""
        if not self.connected or not self.client:
            raise ConnectionError("Not connected to device")

        try:
            # Convert string to bytes if needed
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            # Send data and wait for confirmation
            await self.client.write_gatt_char(
                RX_CHAR_UUID,  # Use RX characteristic for sending data to Arduino
                data,
                response=True
            )
            logger.info(f"Data sent successfully: {data}")
            return True
        except Exception as e:
            logger.error(f"Error sending data: {str(e)}")
            return False

    async def connect_to_device(self, target_device, retry_count=0):
        """Attempt to connect to the device with retry logic."""
        try:
            self.client = BleakClient(target_device.address)
            await self.client.connect()
            logger.info("Connected!")
            
            # Set up indication handler for TX characteristic
            await self.client.start_notify(
                TX_CHAR_UUID,  # Use TX characteristic for receiving data from Arduino
                self.indication_handler
            )
            logger.info("Indication handler set up")
            
            self.connected = True
            self.connection_event.set()
            
            # Keep the connection alive
            while self.connected:
                await asyncio.sleep(1)
                
        except Exception as e:
            logger.error(f"Connection error: {str(e)}")
            self.connected = False
            self.connection_event.clear()
            
            if retry_count < MAX_RETRIES:
                logger.info(f"Retrying in {RETRY_DELAY} seconds... (Attempt {retry_count + 1}/{MAX_RETRIES})")
                await asyncio.sleep(RETRY_DELAY)
                return await self.connect_to_device(target_device, retry_count + 1)
            else:
                logger.error("Max retries reached. Exiting...")
                sys.exit(1)

    async def disconnect(self):
        """Disconnect from the device."""
        if self.client and self.connected:
            await self.client.disconnect()
            self.connected = False
            self.connection_event.clear()
            logger.info("Disconnected from device")

async def scan_for_device():
    """Scan for device with specific service UUID."""
    logger.info("Scanning for device with service UUID: %s", SERVICE_UUID)
    
    while True:
        try:
            # Scan for devices
            devices = await BleakScanner.discover()
            
            # Print all discovered devices
            logger.info("\nDiscovered devices:")
            for device in devices:
                logger.info(f"Device: {device.name or 'Unknown'} - Address: {device.address}")
                if device.name:
                    logger.info(f"  Name: {device.name}")
                if device.metadata.get('uuids'):
                    logger.info(f"  Services: {device.metadata['uuids']}")
                logger.info("---")
            
            # Look for device with our service UUID
            for device in devices:
                if device.metadata.get('uuids') and SERVICE_UUID.lower() in [uuid.lower() for uuid in device.metadata['uuids']]:
                    logger.info(f"\nFound target device: {device.name or 'Unknown'}")
                    logger.info(f"Address: {device.address}")
                    return device
            
            logger.info("Target device not found, scanning again...")
            await asyncio.sleep(1)
            
        except Exception as e:
            logger.error(f"Error during scan: {str(e)}")
            await asyncio.sleep(1)

async def main():
    # Create client instance
    client = KeyBridgeClient()
    
    try:
        # Scan for device
        target_device = await scan_for_device()
        
        # Start connection in background
        connection_task = asyncio.create_task(client.connect_to_device(target_device))
        
        # Wait for connection to be established
        await client.connection_event.wait()
        
        # Example: Send some test data
        await client.send_data("Hello from Python!")
        
        # Keep the program running
        while True:
            await asyncio.sleep(1)
            
    except KeyboardInterrupt:
        logger.info("\nDisconnecting...")
        await client.disconnect()
    except Exception as e:
        logger.error(f"Error in main: {str(e)}")
        await client.disconnect()
    finally:
        sys.exit(0)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("\nExiting...")
        sys.exit(0) 