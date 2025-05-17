#!/bin/bash

# Dynamic detection of Arduino port (normal mode)
find_arduino_port() {
    for port in /dev/cu.usbmodem*; do
        # Skip if the port matches /dev/cu.usbmodemF*
        if [[ "$port" == /dev/cu.usbmodemF* ]]; then
            continue
        fi
        if [ -e "$port" ]; then
            echo "$port"
            return 0
        fi
    done
    return 1
}

# Configuration
#ARDUINO_PORT="/dev/cu.usbmodem3101"
#ARDUINO_PORT="/dev/cu.usbmodem21201"
ARDUINO_PORT=$(find_arduino_port)
BAUD_RATE="115200"
LOG_FILE="arduino_connection.log"
NOTIFIER="/opt/homebrew/bin/terminal-notifier"

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to send notifications
notify() {
    local title="$1"
    local message="$2"
    if [ -f "$NOTIFIER" ]; then
        $NOTIFIER -title "$title" -message "$message" -sound default -timeout 1
    fi
}

# Function to log messages
log() {
    local message="$1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $message" | tee -a "$LOG_FILE"
}

# Function to check if Arduino is connected
check_arduino() {
    if [ -e "$ARDUINO_PORT" ]; then
        return 0
    else
        return 1
    fi
}

# Function to check if we're already connected
is_connected() {
    if lsof "$ARDUINO_PORT" > /dev/null 2>&1; then
        return 0
    fi
    return 1
}

# Function to process a line with color
process_line() {
    local line="$1"
    if [[ $line =~ ^\[(.*?)\]\ ([A-Z]+)\ (.*) ]]; then
        timestamp="${BASH_REMATCH[1]}"
        type="${BASH_REMATCH[2]}"
        content="${BASH_REMATCH[3]}"
        
        # Start with timestamp (no color)
        colored_line="[${timestamp}] "
        
        # Color based on log type
        case $type in
            "INFO")
                colored_line+="${GREEN}INFO${NC} "
                ;;
            "ERROR")
                colored_line+="${RED}ERROR${NC} "
                ;;
            "MEM")
                colored_line+="${BLUE}MEM${NC} "
                ;;
            "WARNING")
                colored_line+="${YELLOW}WARNING${NC} "
                ;;
            "DEBUG")
                colored_line+="${PURPLE}DEBUG${NC} "
                ;;
            *)
                colored_line+="${CYAN}${type}${NC} "
                ;;
        esac
        
        # Add the content
        colored_line+="$content"
        echo -e "$colored_line"
    else
        echo "$line"
    fi
}

# Function to connect to Arduino
connect_arduino() {
    if ! is_connected; then
        log "Connecting to Arduino on $ARDUINO_PORT..."
        stty -f $ARDUINO_PORT $BAUD_RATE cs8 -cstopb -parenb
        if [ $? -eq 0 ]; then
            log "Successfully connected to Arduino"
            notify "Arduino Monitor" "Connected to Arduino"
            # Start reading from the Arduino and display in real-time while also logging
            log "Starting to monitor Arduino output..."
            echo -e "${CYAN}----------------------------------------${NC}"
            echo -e "${CYAN}Arduino Output (Press Ctrl+C to exit):${NC}"
            echo -e "${CYAN}----------------------------------------${NC}"
            
            # Read from Arduino and process lines directly
            stdbuf -oL cat $ARDUINO_PORT | while read -r line; do
                # Log the raw line to file
                echo "$line" >> arduino_output.log
                # Process and display the colored line
                process_line "$line"
            done &
            
            return 0
        else
            log "Failed to connect to Arduino"
            return 1
        fi
    fi
    return 0
}

# Cleanup function
cleanup() {
    log "Cleaning up and exiting..."
    pkill -f "cat $ARDUINO_PORT" 2>/dev/null
    exit 0
}

# Set up cleanup on script exit
trap cleanup EXIT

# Main monitoring loop
log "Starting Arduino monitor..."
notify "Arduino Monitor" "Starting monitor for $ARDUINO_PORT"

# Cleanup any existing background processes
pkill -f "cat $ARDUINO_PORT" 2>/dev/null

while true; do
    ARDUINO_PORT=$(find_arduino_port)
    if [ -n "$ARDUINO_PORT" ]; then
        connect_arduino
    else
        log "No Arduino port found. Retrying..."
    fi
    sleep 2
done 
