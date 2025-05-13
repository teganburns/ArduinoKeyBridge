#include <ArduinoBLE.h>

// Device name
#define DEVICE_NAME "ArduinoKeyBridge"

// Service and Characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// BLE objects
BLEService keyService(SERVICE_UUID);
BLEByteCharacteristic keyCharacteristic(CHARACTERISTIC_UUID, BLEWrite | BLENotify);

// Buffer for serial input
const int BUFFER_SIZE = 32;
char inputBuffer[BUFFER_SIZE];
int bufferIndex = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1);
    }

    // Set advertised local name and service UUID
    BLE.setLocalName(DEVICE_NAME);
    BLE.setAdvertisedService(keyService);

    // Add the characteristic to the service
    keyService.addCharacteristic(keyCharacteristic);

    // Add service
    BLE.addService(keyService);

    // Set the initial value for the characteristic
    keyCharacteristic.writeValue(0);

    // Start advertising
    BLE.advertise();
    Serial.println("BLE device active, waiting for connections...");
    Serial.println("Type text in Serial Monitor to send over BLE");
}

void loop() {
    // Listen for BLE peripherals to connect
    BLEDevice central = BLE.central();

    // If a central is connected to peripheral
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());

        // While the central is still connected to peripheral
        while (central.connected()) {
            // Check for incoming BLE data
            if (keyCharacteristic.written()) {
                byte value = keyCharacteristic.value();
                Serial.print("Received data: ");
                Serial.println(value, HEX);
                keyCharacteristic.writeValue(value);
            }

            // Check for serial input
            while (Serial.available() > 0) {
                char c = Serial.read();
                
                // If we get a newline, process the buffer
                if (c == '\n') {
                    inputBuffer[bufferIndex] = '\0';  // Null terminate the string
                    
                    // Convert the input string to bytes and send
                    for (int i = 0; i < bufferIndex; i++) {
                        keyCharacteristic.writeValue(inputBuffer[i]);
                        delay(10);  // Small delay between characters
                    }
                    
                    // Reset buffer
                    bufferIndex = 0;
                    Serial.println("Sent!");
                }
                // Otherwise add to buffer if there's space
                else if (bufferIndex < BUFFER_SIZE - 1) {
                    inputBuffer[bufferIndex++] = c;
                }
            }
        }

        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
} 