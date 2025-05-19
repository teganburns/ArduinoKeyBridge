import socket
from key_report import KeyReport
from log import get_logger
import threading
import time
from collections import deque

logger = get_logger(__name__)

class KeyBridgeTCPServer:
    """
    Handles TCP networking for ArduinoKeyBridge.
    Provides methods to connect, send, receive, and close the connection.
    Also manages connection state, send timing, and thread-safe sending.
    """
    def __init__(self, host: str, port: int):
        self.host = host
        self.port = port
        self.sock = None
        self.connected = False
        self.send_lock = threading.Lock()
        self.send_times = {}  # message -> send_time
        self._report_queue = deque()

    def connect(self):
        try:
            self.sock = socket.create_connection((self.host, self.port))
            return True
        except Exception as e:
            logger.error("Connection error: %s", e)
            self.sock = None
            return False

    def connect_and_init(self) -> bool:
        """
        Connect to the server and send an initial empty key report.
        Returns True if successful, False otherwise.
        """
        logger.info("Connecting to Arduino at %s:%s...", self.host, self.port)
        if self.connect():
            self.connected = True
            logger.info("Connected to Arduino TCP server!")
            # Send an empty key report after connecting (8 bytes of zeros)
            self.send_key_report(bytes([0x00] * 8))
            logger.info("Sent empty key report to Arduino after connecting.")
            return True
        else:
            logger.error("TCP connection failed.")
            self.connected = False
            return False

    def receive_data(self):
        """
        Generator: Yields each KeyReport received. Runs synchronously and blocks until the connection is closed or an error occurs.
        """
        while self.connected:
            try:
                report = self.receive_key_report()
                if report:
                    if isinstance(report, KeyReport):
                        logger.info("Received KeyReport: %r", report)
                        yield report
                    else:
                        logger.warning("Ignored non-KeyReport packet: %r", report)
                else:
                    logger.error("Connection closed by Arduino")
                    self.connected = False
                    break
            except Exception as e:
                logger.error("Error receiving data: %s", e)
                self.connected = False
                break

    def send_key_report(self, key_report):
        """
        Thread-safe send of a key report (8 bytes). Also sends a key release after.
        """
        if not isinstance(key_report, bytes) or len(key_report) != 8:
            logger.error("send_key_report: key_report must be 8 bytes")
            return
        try:
            with self.send_lock:
                self.send_times[key_report] = time.time()
            if self.sock:
                self.sock.sendall(key_report)
                logger.info("Sent key report: %s", key_report.hex(' '))
                # Always send an empty key report after to release the key
                empty_report = bytes([0x00] * 8)
                self.sock.sendall(empty_report)
                logger.info("Sent key release report: %s", empty_report.hex(' '))
            else:
                logger.error("Socket is not connected.")
        except Exception as e:
            logger.error("Error sending data: %s", e)
            self.connected = False

    def send_string(self, string):
        """
        Send a string of key reports.
        """
        # First send the charter mode key report (0x22 modifier and 0x2 for all keys)
        charter_report = KeyReport(0x22, [0x2, 0x2, 0x2, 0x2, 0x2, 0x2])
        self.send_key_report(charter_report.to_bytes())
        time.sleep(0.1)

        try:
            with self.send_lock:
                self.send_times[string] = time.time()
            if self.sock:
                self.sock.sendall(string.encode() + b'\0')
                logger.info("Sent string: %s", string)
            else:
                logger.error("Socket is not connected.")
        except Exception as e:
            logger.error("Error sending data: %s", e)
            self.connected = False

    def receive_key_report(self) -> KeyReport:
        try:
            data = self.sock.recv(8)
            if len(data) == 8:
                return KeyReport.from_bytes(data)
            else:
                logger.warning("Received incomplete key report: %s", data)
                return None
        except Exception as e:
            logger.error("Receive error: %s", e)
            self.reconnect()
            return None

    def reconnect(self):
        logger.warning("Attempting to reconnect...")
        self.close()
        self.connect()

    def close(self):
        if self.sock:
            try:
                self.sock.close()
            except Exception:
                pass
            self.sock = None

    def begin(self):
        """
        Start the server connection and perform any initial setup.
        Loops until connection and initialization succeed.
        """
        while True:
            if self.connect_and_init():
                break
            logger.error("Retrying connection in 2 seconds...")
            time.sleep(2)

    def poll(self):
        """
        Check for new data and queue KeyReport if available. Non-blocking.
        """
        if self.connected:
            report = self.receive_key_report()
            if isinstance(report, KeyReport):
                logger.info("Polled KeyReport: %r", report)
                self._report_queue.append(report)

    def has_new_report(self):
        """
        Return True if there is a new KeyReport in the queue.
        """
        return bool(self._report_queue)

    def get_next_report(self):
        """
        Pop and return the next KeyReport from the queue, or None if empty.
        """
        if self._report_queue:
            return self._report_queue.popleft()
        return None 