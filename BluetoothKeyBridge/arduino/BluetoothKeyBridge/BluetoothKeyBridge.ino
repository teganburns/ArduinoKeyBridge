#include <ArduinoBLE.h>

// Device name
#define DEVICE_NAME "ArduinoKeyBridge"

// Service and Characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TX_CHAR_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26a8"  // Arduino → Central
#define RX_CHAR_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26a9"  // Central → Arduino

// Buffer sizes
const int BUFFER_SIZE = 512;  // Increased buffer size for more data

// BLE objects
BLEService keyService(SERVICE_UUID);
// Update permissions to allow bidirectional communication
BLECharacteristic txCharacteristic(TX_CHAR_UUID, BLERead | BLEWrite | BLEIndicate, BUFFER_SIZE);
BLECharacteristic rxCharacteristic(RX_CHAR_UUID, BLEWrite | BLENotify, BUFFER_SIZE);

// Buffer for serial input
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

    // Add characteristics to the service
    keyService.addCharacteristic(txCharacteristic);
    keyService.addCharacteristic(rxCharacteristic);

    // Add service
    BLE.addService(keyService);

    // Set initial values
    uint8_t initialValue = 0;
    txCharacteristic.writeValue(initialValue);
    rxCharacteristic.writeValue(initialValue);

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
            // Check for incoming BLE data on both characteristics
            if (rxCharacteristic.written()) {
                // Get the received data
                const uint8_t* data = rxCharacteristic.value();
                int dataLength = rxCharacteristic.valueLength();
                
                Serial.print("Received data on RX (");
                Serial.print(dataLength);
                Serial.println(" bytes):");
                
                // Print received data as hex
                for (int i = 0; i < dataLength; i++) {
                    if (data[i] < 0x10) Serial.print("0");
                    Serial.print(data[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
                
                // Send acknowledgment through TX characteristic
                txCharacteristic.writeValue(data, dataLength);
            }

            if (txCharacteristic.written()) {
                // Get the received data
                const uint8_t* data = txCharacteristic.value();
                int dataLength = txCharacteristic.valueLength();
                
                Serial.print("Received data on TX (");
                Serial.print(dataLength);
                Serial.println(" bytes):");
                
                // Print received data as hex
                for (int i = 0; i < dataLength; i++) {
                    if (data[i] < 0x10) Serial.print("0");
                    Serial.print(data[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            }

            // Check for serial input
            while (Serial.available() > 0) {
                char c = Serial.read();
                
                // If we get a newline, process the buffer
                if (c == '\n') {
                    inputBuffer[bufferIndex] = '\0';  // Null terminate the string
                    
                    // Send the complete message through TX characteristic
                    txCharacteristic.writeValue((uint8_t*)inputBuffer, bufferIndex);
                    
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