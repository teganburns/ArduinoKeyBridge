#!/bin/bash

# Dynamic detection of Arduino port (normal mode)
find_arduino_port() {
    for port in /dev/cu.usbmodemF*; do
        if [ -e "$port" ]; then
            echo "$port"
            return 0
        fi
    done
    return 1
}
# Get the absolute path to the script's directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set the project root to the standard ArduinoKeyBridge directory
echo "🔸 Using ArduinoKeyBridge"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/../../ArduinoKeyBridge" && pwd )"

#PROJECT_ROOT="$( cd "$SCRIPT_DIR/../../ArduinoKeyBridgeTCP/arduino/ClientConnectionDemo" && pwd )" 

# Important: We use /tmp/arduino_build to avoid potential permission/caching issues
# that can occur with local build directories. This ensures a clean build every time.
BUILD_PATH="/tmp/arduino_build"

# Port 3101 is the normal running state port - we only upload on other ports
#NORMAL_PORT_PATTERN=$(find_arduino_port)
FQBN="arduino:renesas_uno:unor4wifi"
NOTIFIER=$(which terminal-notifier)

# Check if terminal-notifier is installed
if [ -z "$NOTIFIER" ]; then
    echo "❌ terminal-notifier is not installed."
    echo "To install it, run:"
    echo "brew install terminal-notifier"
    echo ""
    echo "Would you like to continue without notifications? (y/n)"
    read -r response
    if [[ "$response" =~ ^[Nn]$ ]]; then
        echo "Please install terminal-notifier and try again."
        exit 1
    else
        echo "Continuing without notifications..."
        NOTIFIER=""
    fi
fi

# Function to send notifications
notify() {
    local title="$1"
    local message="$2"
    if [ -n "$NOTIFIER" ]; then
        $NOTIFIER -title "$title" -message "$message" -sound default -timeout 1
    fi
}

# Function to format time duration
format_duration() {
    local duration=$1
    local mins=$((duration / 60))
    local secs=$((duration % 60))
    if [ "$mins" -gt "0" ]; then
        echo "${mins}m ${secs}s"
    else
        echo "${secs}s"
    fi
}

# Function to print timing info in a pretty format
print_timing() {
    local step=$1
    local duration=$2
    local formatted_time=$(format_duration $duration)
    printf "⏱️  %-20s %15s\n" "$step" "$formatted_time"
}

# Ensure we're in the project root directory
cd "$PROJECT_ROOT"
echo "📂 Working directory: $(pwd)"
echo "🔍 Monitoring for Arduino UNO R4 WiFi..."
echo "Double press the reset button when you want to upload."
echo "Press Ctrl+C to exit."

while true; do
    # Wait for Arduino programming port
    PROG_PORT=$(find_arduino_port)
    if [ -n "$PROG_PORT" ]; then
        echo "✨ Arduino detected on programming port: $PROG_PORT"
        notify "Arduino Reset" "Reset detected on port $PROG_PORT"
        # Start timing the entire process
        TOTAL_START=$(date +%s)
        echo "🔨 Compiling sketch..."
        notify "Arduino Upload" "Starting compilation"
        # Clean the temp build directory to ensure fresh build
        CLEAN_START=$(date +%s)
        echo "🧹 Cleaning temp build directory..."
        rm -rf "$BUILD_PATH"
        mkdir -p "$BUILD_PATH"
        CLEAN_END=$(date +%s)
        CLEAN_DURATION=$((CLEAN_END - CLEAN_START))
        # Show what we're about to compile
        echo "📝 Compiling files in: $(pwd)"
        ls -la *.ino *.cpp *.h
        # Compile with clean build to temp directory
        COMPILE_START=$(date +%s)
        echo "🏗️ Building to temp directory: $BUILD_PATH"
        arduino-cli compile --clean --build-path "$BUILD_PATH" --fqbn $FQBN .
        COMPILE_RESULT=$?
        COMPILE_END=$(date +%s)
        COMPILE_DURATION=$((COMPILE_END - COMPILE_START))
        if [ $COMPILE_RESULT -eq 0 ]; then
            echo "✅ Compilation successful!"
            notify "Arduino Upload" "Compilation successful, starting upload"
            echo "📤 Uploading sketch from temp build directory..."
            # Upload from temp build directory
            UPLOAD_START=$(date +%s)
            arduino-cli upload -p $PROG_PORT --fqbn $FQBN --input-dir "$BUILD_PATH" .
            UPLOAD_RESULT=$?
            UPLOAD_END=$(date +%s)
            UPLOAD_DURATION=$((UPLOAD_END - UPLOAD_START))
            if [ $UPLOAD_RESULT -eq 0 ]; then
                echo "✅ Upload successful!"
                notify "Arduino Upload" "Upload completed successfully"
                # Clean up temp build directory
                CLEANUP_START=$(date +%s)
                echo "🧹 Cleaning up temp build directory..."
                rm -rf "$BUILD_PATH"
                CLEANUP_END=$(date +%s)
                CLEANUP_DURATION=$((CLEANUP_END - CLEANUP_START))
                # Calculate total time
                TOTAL_END=$(date +%s)
                TOTAL_DURATION=$((TOTAL_END - TOTAL_START))
                # Print timing summary
                echo ""
                echo "⏰ Timing Summary:"
                echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
                print_timing "Initial Cleanup" $CLEAN_DURATION
                print_timing "Compilation" $COMPILE_DURATION
                print_timing "Upload" $UPLOAD_DURATION
                print_timing "Final Cleanup" $CLEANUP_DURATION
                echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
                print_timing "Total Time" $TOTAL_DURATION
                echo "📅 Completed at: $(date '+%Y-%m-%d %I:%M:%S %p')"
                echo ""
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
        # Wait for the port to disappear before next upload
        while [ -n "$(find_arduino_port)" ]; do
            sleep 2
        done
    fi
    # Sleep briefly to prevent high CPU usage
    sleep 1
done 
