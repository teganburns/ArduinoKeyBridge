"""
log.py
------
Logging setup for ArduinoKeyBridgeServer.
Provides a configured logger for use throughout the package.
"""
import logging
import sys

LOG_FORMAT = "[%(asctime)s] %(levelname)s: %(message)s"
DATE_FORMAT = "%H:%M:%S"

# Optional: Color support for terminal output
try:
    from colorama import init, Fore, Style
    init(autoreset=True)
    class ColorFormatter(logging.Formatter):
        COLORS = {
            logging.DEBUG: Fore.CYAN,
            logging.INFO: Fore.GREEN,
            logging.WARNING: Fore.YELLOW,
            logging.ERROR: Fore.RED,
            logging.CRITICAL: Fore.MAGENTA + Style.BRIGHT,
        }
        def format(self, record):
            color = self.COLORS.get(record.levelno, "")
            message = super().format(record)
            return f"{color}{message}{Style.RESET_ALL}"
    _formatter = ColorFormatter(LOG_FORMAT, DATE_FORMAT)
except ImportError:
    _formatter = logging.Formatter(LOG_FORMAT, DATE_FORMAT)

_logger = logging.getLogger("ArduinoKeyBridgeServer")
_logger.setLevel(logging.INFO)
if not _logger.hasHandlers():
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(_formatter)
    _logger.addHandler(handler)


def get_logger(name=None):
    """
    Get a logger for the given module name, or the package logger by default.
    """
    if name:
        return _logger.getChild(name)
    return _logger


def set_log_level(level):
    """
    Set the log level for the package logger.
    """
    _logger.setLevel(level)
    for handler in _logger.handlers:
        handler.setLevel(level) 