#!/bin/bash

# Get the absolute path to the script's directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Important: We use /tmp/arduino_build to avoid potential permission/caching issues
# that can occur with local build directories. This ensures a clean build every time.
BUILD_PATH="/tmp/arduino_build"

# Port 3101 is the normal running state port - we only upload on other ports
NORMAL_PORT_PATTERN="3101"
FQBN="arduino:renesas_uno:unor4wifi"
NOTIFIER="/opt/homebrew/bin/terminal-notifier"

# Function to send notifications
notify() {
    local title="$1"
    local message="$2"
    $NOTIFIER -title "$title" -message "$message" -sound default -timeout 1
}

# Ensure we're in the correct directory
cd "$SCRIPT_DIR"
echo "📂 Working directory: $(pwd)"
echo "🔍 Monitoring for Arduino UNO R4 WiFi..."
echo "Double press the reset button when you want to upload."
echo "Press Ctrl+C to exit."

while true; do
    # Get all Arduino ports
    ARDUINO_PORTS=$(ls /dev/cu.usbmodem* 2>/dev/null || true)
    
    # Check if we have an Arduino port that's not the normal running port
    for port in $ARDUINO_PORTS; do
        if [[ $port != *"$NORMAL_PORT_PATTERN"* ]]; then
            echo "✨ Arduino detected on reset port: $port"
            notify "Arduino Reset" "Reset detected on port $port"
            
            echo "🔨 Compiling sketch..."
            notify "Arduino Upload" "Starting compilation"
            
            # Clean the temp build directory to ensure fresh build
            echo "🧹 Cleaning temp build directory..."
            rm -rf "$BUILD_PATH"
            mkdir -p "$BUILD_PATH"
            
            # Show what we're about to compile
            echo "📝 Compiling files in: $(pwd)"
            ls -la *.ino *.cpp *.h
            
            # Compile with clean build to temp directory
            # Using --build-path ensures we avoid any local build caching issues
            echo "🏗️ Building to temp directory: $BUILD_PATH"
            arduino-cli compile --clean --build-path "$BUILD_PATH" --fqbn $FQBN "$SCRIPT_DIR"
            COMPILE_RESULT=$?
            
            if [ $COMPILE_RESULT -eq 0 ]; then
                echo "✅ Compilation successful!"
                notify "Arduino Upload" "Compilation successful, starting upload"
                echo "📤 Uploading sketch from temp build directory..."
                
                # Upload from temp build directory
                arduino-cli upload -p $port --fqbn $FQBN --input-dir "$BUILD_PATH" "$SCRIPT_DIR"
                UPLOAD_RESULT=$?
                
                if [ $UPLOAD_RESULT -eq 0 ]; then
                    echo "✅ Upload successful!"
                    notify "Arduino Upload" "Upload completed successfully"
                    
                    # Clean up temp build directory
                    echo "🧹 Cleaning up temp build directory..."
                    rm -rf "$BUILD_PATH"
                else
                    echo "❌ Upload failed!"
                    notify "Arduino Upload" "Upload failed with error $UPLOAD_RESULT"
                fi
            else
                echo "❌ Compilation failed!"
                notify "Arduino Upload" "Compilation failed with error $COMPILE_RESULT"
            fi
            
            echo "🔄 Returning to monitoring mode..."
            echo "Double press the reset button when you want to upload again."
            break
        fi
    done
    
    # Sleep briefly to prevent high CPU usage
    sleep 1
done 