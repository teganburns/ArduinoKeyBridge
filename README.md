**Build Status:**

- **main:**
  [![Compile Arduino Sketches (Uno R4 WiFi) - main](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml/badge.svg?branch=main)](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml)
- **development:**
  [![Compile Arduino Sketches (Uno R4 WiFi) - development](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml/badge.svg?branch=development)](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml)
- **chatgpt-codex:**
  [![Compile Arduino Sketches (Uno R4 WiFi) - chatgpt-codex](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml/badge.svg?branch=chatgpt-codex)](https://github.com/teganburns/ArduinoKeyBridge/actions/workflows/compile-arduino-unor4wifi.yml)

# ArduinoKeyBridge

**ArduinoKeyBridge** transforms ordinary keyboards into powerful, standalone input devices with customizable functionality. Designed for innovation and control, this project allows users to inject custom code and assign special operations to keys—all without relying on a connected computer. With integrated Wi-Fi, your keyboard becomes a fully autonomous device capable of advanced interactions and remote command execution.

## Features
- **Customizable Key Functions**: Modify any key to perform specific operations, from launching commands to sending complex inputs—completely independent of the host device.
- **Wi-Fi Integration**: Send and receive commands wirelessly, enabling real-time interaction and control over networked systems.
- **Standalone Operation**: No need to run software on a connected computer—your keyboard handles everything on its own.
- **Apple Magic Keyboard Support**: Out-of-the-box compatibility with Apple Magic Keyboard 2 (with number pad and fingerprint reader), with flexibility to work with most other keyboards.
- **Secure and Purpose-Driven**: Built for legitimate use cases like automation, accessibility enhancements, and creative workflows—not for unethical purposes.
- **Modular Codebase**: Easily adapt the framework for your specific needs with clear and extensible components.

## Requirements
This project was developed and tested using the following hardware. It is recommended to use the same or equivalent components for optimal compatibility:

1. **[Apple Magic Keyboard 2 with Number Pad and Fingerprint Reader](https://www.apple.com/shop/product/MXK83LL/A/magic-keyboard-with-touch-id-and-numeric-keypad-for-mac-models-with-apple-silicon-usb-c-us-english-black-keys)**  
   *Note: Not all functions (e.g., fingerprint reader) are fully supported yet.*

2. **[SparkFun HID Keyboard Shield (Version 2)](https://www.sparkfun.com/products/21247)**  
   *An essential shield for interfacing with USB keyboards.*

3. **[Arduino Uno Revision 4 Wi-Fi Board](https://store.arduino.cc/products/uno-r4-wifi?queryID=undefined)**  
   *Provides processing power and Wi-Fi connectivity.*

## Development Tools
The project includes various development tools to streamline the development process:

- [Development Tools Documentation](docs/tools.md) - Documentation for development scripts and utilities
  - Upload Monitor Script for automated compilation and uploading
  - Additional development tools and scripts

## Use Cases
- **Custom Keyboard Shortcuts**: Create powerful macros or actions triggered by specific keys.
- **IoT and Automation**: Use your keyboard as a control hub for smart devices or automated systems.
- **Remote Interactions**: Send commands to the device via Wi-Fi and let it perform tasks independently.
- **Accessibility Enhancements**: Develop innovative solutions for users with specialized input needs.

Whether you're building a smarter workspace, experimenting with IoT, or developing custom tools, **ArduinoKeyBridge** empowers your keyboard with supercharged capabilities. Contributions and ideas are welcome to push the boundaries of what's possible!
