import requests
import base64
from config import *


class ChatGPTClient:
    """
    Handles requests to the ChatGPT API.
    """
    def __init__(self):
        from config import CHATGPT_API_KEY
        self.api_key = CHATGPT_API_KEY
        self.endpoint = CHATGPT_ENDPOINT
        self.model = CHATGPT_MODEL
        self.max_tokens = CHATGPT_MAX_TOKENS
        self.temperature = CHATGPT_TEMPERATURE

    def send_prompt(self, prompt, temperature=CHATGPT_TEMPERATURE, max_tokens=CHATGPT_MAX_TOKENS):
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        data = {
            "model": CHATGPT_MODEL,
            "messages": [
                {"role": "user", "content": prompt}
            ],
            "temperature": temperature,
            "max_tokens": max_tokens
        }
        response = requests.post(self.endpoint, headers=headers, json=data)
        if response.status_code == 200:
            return response.json()["choices"][0]["message"]["content"]
        else:
            raise Exception(f"ChatGPT API request failed: {response.status_code} {response.text}")

    @staticmethod
    def file_to_base64(file_path):
        """
        Reads a file from the given path and returns its base64-encoded string.
        """
        with open(file_path, "rb") as f:
            encoded = base64.b64encode(f.read()).decode('utf-8')
        return encoded 

    def send_image_with_prompt(self, image_paths, prompt=DEFAULT_PROMPT):
        """
        Send one or more images to ChatGPT with a prompt. image_paths must be a list of strings (file paths), or a single string (which will be converted to a list).
        """
        if isinstance(image_paths, str):
            image_paths = [image_paths]
        if not isinstance(image_paths, list) or not all(isinstance(p, str) for p in image_paths):
            raise ValueError("image_paths must be a list of strings or a single string")
        base64_images = [self.file_to_base64(path) for path in image_paths]
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        content = [
            {"type": "text", "text": prompt}
        ]
        for b64 in base64_images:
            content.append({"type": "image_url", "image_url": {"url": f"data:image/png;base64,{b64}"}})
        payload = {
            "model": CHATGPT_MODEL,
            "messages": [
                {
                    "role": "user",
                    "content": content
                }
            ],
            "max_tokens": 300
        }
        response = requests.post(
            self.endpoint,
            headers=headers,
            json=payload
        )
        return response.json() 