# config.py
# Centralized configuration for ArduinoKeyBridge Python client
import os

ARDUINO_HOST = '192.168.4.1'
ARDUINO_PORT = 8080
ARDUINO_MAC = 'f2:f5:bd:4f:87:28'
STATUS_RETRY_COUNT = 3


# Get API keys from environment variables
CHATGPT_API_KEY = os.getenv('CHATGPT_API_KEY')

# Base data directory
DATA_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '../data'))

# ChatGPT directories
CHATGPT_COLLECTION_NAME = 'chatgpt'
CHATGPT_DIR = os.path.join(DATA_DIR, 'chatgpt')
CHATGPT_ACTIVE_DIR = os.path.join(CHATGPT_DIR, 'active')
CHATGPT_ARCHIVE_DIR = os.path.join(CHATGPT_DIR, 'archive')

# Screenshots directories
SCREENSHOT_COLLECTION_NAME = 'screenshot'
SCREENSHOT_DIR = os.path.join(DATA_DIR, 'screenshot')
SCREENSHOT_ACTIVE_DIR = os.path.join(SCREENSHOT_DIR, 'active')
SCREENSHOT_ARCHIVE_DIR = os.path.join(SCREENSHOT_DIR, 'archive')

# Database directory
DATABASE_DIR = os.path.join(DATA_DIR, 'database')
DATABASE_FILE = os.path.join(DATABASE_DIR, 'data.db')

# MongoDB
MONGODB_URI = 'mongodb://localhost:27017/'
DATABASE_NAME = 'arduino_keybridge'

# Add other configuration variables here as needed 

