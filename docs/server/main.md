# main.py

Starts the TCP client and dispatches key reports to `CommandHandler`.

## Flow

1. Print a startup message and locate the Arduino IP.
2. Create `KeyBridgeTCPServer` and `CommandHandler` instances.
3. Enter a loop reading key reports and passing them to the handler.
