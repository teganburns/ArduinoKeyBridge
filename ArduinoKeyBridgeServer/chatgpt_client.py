import requests
import base64

class ChatGPTClient:
    """
    Handles requests to the ChatGPT API.
    """
    def __init__(self, api_key, endpoint="https://api.openai.com/v1/chat/completions"):
        self.api_key = api_key
        self.endpoint = endpoint

    def send_prompt(self, prompt, model="gpt-3.5-turbo", temperature=0.7, max_tokens=256):
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        data = {
            "model": model,
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