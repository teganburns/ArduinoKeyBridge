# chatgpt_client.py

Utility class for sending prompts and images to the ChatGPT API as well as converting text to speech.

## Key Methods

- `send_prompt(prompt)` – Submit a text prompt and return the response text.
- `send_image_with_prompt(image_paths, prompt)` – Send one or more images with a prompt.
- `file_to_base64(path)` – Helper used by `send_image_with_prompt()`.
- `text_to_speech(text, voice, model, output_path)` – Use OpenAI's TTS API to save speech audio.
