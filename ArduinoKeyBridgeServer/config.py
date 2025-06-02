# config.py
# Centralized configuration for ArduinoKeyBridge Python client
import os

ARDUINO_HOST = '192.168.4.1'
ARDUINO_PORT = 8080
ARDUINO_MAC = 'f2:f5:bd:4f:87:28'
STATUS_RETRY_COUNT = 3


# Get API keys from environment variables
#CHATGPT_API_KEY = os.getenv('OPENAI_API_KEY')
CHATGPT_API_KEY = "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"
CHATGPT_MODEL = "gpt-4o"
CHATGPT_MAX_TOKENS = 256
CHATGPT_TEMPERATURE = 0.7
CHATGPT_ENDPOINT = "https://api.openai.com/v1/chat/completions"


DEFAULT_PROMPT= "Look at the images and only provide the correct answer to the question. If it is a coding question only provide the code. Do not provide any explnation or other text. If it multiple choice, provide the letter of the correct answer."
DEFAULT_PROMPT_MULTIPLE_CHOICE= "Look at the images and only provide the correct answer to the question. Do not provide any explnation or other text. This is a multiple choice question only provide the letter of the correct answer."
DEFAULT_PROMPT_CODE =  "Look at the images and only provide the correct answer to the question. Do not provide any explnation or other text. Do not repeat what is in the code block or the comments. This is a coding question only provide the code of the correct answer."



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

