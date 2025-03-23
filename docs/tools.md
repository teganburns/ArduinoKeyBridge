# Development Tools

This directory contains various development tools and scripts to assist with the ArduinoKeyBridge project.

## Upload Monitor Script

Located at `tools/bash/upload_monitor.sh`, this script provides an automated way to compile and upload code to the Arduino UNO R4 WiFi board.

### Features

- Automatically detects when the Arduino is in bootloader mode
- Provides desktop notifications for important events
- Shows detailed timing information for each step
- Uses a clean build directory to avoid caching issues
- Supports the Arduino UNO R4 WiFi board

### Prerequisites

- macOS (uses `terminal-notifier` for notifications)
- `arduino-cli` installed and configured
- Homebrew (for `terminal-notifier`)

### Installation

1. Install terminal-notifier if not already installed:
   ```bash
   brew install terminal-notifier
   ```

2. Make the script executable:
   ```bash
   chmod +x tools/bash/upload_monitor.sh
   ```

### Usage

1. Navigate to your Arduino project directory
2. Run the script:
   ```bash
   ./tools/bash/upload_monitor.sh
   ```
3. Double-press the reset button on your Arduino when you want to upload
4. The script will:
   - Detect the Arduino in bootloader mode
   - Clean the build directory
   - Compile the sketch
   - Upload the compiled code
   - Show timing information
   - Return to monitoring mode

### Output

The script provides detailed feedback including:
- Working directory information
- Compilation status
- Upload status
- Timing information for each step
- Desktop notifications for important events

### Timing Information

The script shows timing for:
- Initial cleanup
- Compilation
- Upload
- Final cleanup
- Total time

### Exiting

Press `Ctrl+C` to exit the monitoring script.

### Notes

- The script uses `/tmp/arduino_build` for temporary build files
- It monitors for ports that don't match the normal running state port (3101)
- Uses the FQBN `arduino:renesas_uno:unor4wifi` for the Arduino UNO R4 WiFi 