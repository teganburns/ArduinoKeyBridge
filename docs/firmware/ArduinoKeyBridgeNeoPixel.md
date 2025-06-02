# ArduinoKeyBridgeNeoPixel (ArduinoKeyBridgeNeoPixel.h / ArduinoKeyBridgeNeoPixel.cpp)

Wrapper around the `Adafruit_NeoPixel` library providing status indicators and animations.

## Key Methods

- `begin(pin, numPixels)` – Allocate the strip and set initial brightness.
- `setColor()` / `setPixelColors()` – Set solid or per-pixel colors.
- `setStatus*()` – Convenience helpers for idle, busy, error and success states.
- `showSetupProgress(progress)` – Display setup progress using white pixels.
- `rollColor(delayMs)` – Animation that rolls the current color across the strip.

The singleton is accessed from other modules to signal connection status and errors.
