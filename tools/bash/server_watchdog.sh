#!/bin/bash

# Watchdog script for ArduinoKeyBridgeServer
# Restarts the Python server if the Arduino is unreachable

ARDUINO_IP="192.168.4.1"
PYTHON_SCRIPT="main.py"
PYTHON_PATH="/Users/teganburns/Documents/Mac Mini - Documents/Development/ArduinoKeyBridge_cursor/ArduinoKeyBridgeServer"
VENV_PATH="/Users/teganburns/Documents/Mac Mini - Documents/Development/ArduinoKeyBridge_cursor/venv/bin/activate"
LOGFILE="$PYTHON_PATH/server_watchdog.log"
PID_FILE="$PYTHON_PATH/server.pid"

# Function to log messages with timestamp
log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOGFILE"
}

# Function to check if Python server is running
check_server() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if ps -p $PID > /dev/null; then
            # Verify it's our Python process
            if ps -p $PID -o command | grep -q "$PYTHON_SCRIPT"; then
                return 0
            fi
        fi
    fi
    return 1
}

# Function to kill the server
kill_server() {
    log_message "Attempting to kill server..."
    pkill -f "$PYTHON_SCRIPT"
    sleep 2
    while check_server; do
        log_message "Server still running, killing again..."
        pkill -9 -f "$PYTHON_SCRIPT"
        sleep 2
    done
    log_message "Server killed successfully"
}

cd "$PYTHON_PATH"
log_message "Watchdog started - Monitoring Arduino at $ARDUINO_IP"

# Kill and restart the server if it is already running
if check_server; then
    log_message "Server is already running. Killing and restarting..."
    kill_server
    nohup bash -c "source '$VENV_PATH' && python3 '$PYTHON_SCRIPT'" >> "$LOGFILE" 2>&1 &
    echo $! > "$PID_FILE"
    log_message "Server restart initiated"
    sleep 10  # Give the server time to start
    if check_server; then
        log_message "Server successfully restarted"
    else
        log_message "WARNING: Server failed to restart"
    fi
fi

while true; do
    # Check Arduino connection with a short timeout
    ping -c 1 -W 1 "$ARDUINO_IP" > /dev/null 2>&1
    ARDUINO_STATUS=$?
    
    # Check Python server
    check_server
    SERVER_STATUS=$?
    
    # Log status only if there is a failure
    if [ $ARDUINO_STATUS -ne 0 ]; then
        log_message "Arduino: Not reachable"
        kill_server
        nohup bash -c "source '$VENV_PATH' && python3 '$PYTHON_SCRIPT'" >> "$LOGFILE" 2>&1 &
        echo $! > "$PID_FILE"
        log_message "Server restart initiated"
        sleep 10  # Give the server time to start
        if check_server; then
            log_message "Server successfully restarted"
        else
            log_message "WARNING: Server failed to restart"
        fi
    fi
    
    if [ $SERVER_STATUS -ne 0 ]; then
        log_message "Server: Not running"
        log_message "Attempting to start server..."
        # Activate venv and start server
        nohup bash -c "source '$VENV_PATH' && python3 '$PYTHON_SCRIPT'" >> "$LOGFILE" 2>&1 &
        echo $! > "$PID_FILE"
        log_message "Server start initiated"
        sleep 10  # Give the server time to start
        
        # Verify server started
        check_server
        if [ $? -eq 0 ]; then
            log_message "Server successfully started"
        else
            log_message "WARNING: Server failed to start"
        fi
    fi
done 