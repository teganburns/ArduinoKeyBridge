# ArduinoKeyBridgeLogger (ArduinoKeyBridgeLogger.h / ArduinoKeyBridgeLogger.cpp)

Simple serial logger used across the firmware. Supports multiple log levels and memory diagnostics.

## Key Methods

- `begin(baudRate)` – Initialize SerialUSB and set the start time.
- `setLogLevel(level)` – Change the active log level.
- `debug()/info()/warning()/error()` – Standard logging methods accepting a source tag and message.
- `mem()` – Log memory usage and call `logMemory()`.
- `hexDump(data, length)` – Output a hexadecimal dump for debugging.
- `logMemory(source)` – Measure available SRAM and print usage statistics.
- `findMaxAllocation()` – Determine the largest allocatable memory block.

Convenience macros like `LOG_DEBUG` wrap these methods for easy use.
