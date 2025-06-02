# BluetoothKeyBridge

This folder contains an experimental Bluetooth Low Energy (BLE) variant of the project.

- **`arduino/`** – Arduino sketch implementing a simple BLE service that echoes data between a central device and the board.
- **`python/`** – Python client using the `bleak` library to discover the board and exchange messages.

The BLE version was used for early throughput tests and is kept here for reference. The main project uses Wi‑Fi instead of BLE.
