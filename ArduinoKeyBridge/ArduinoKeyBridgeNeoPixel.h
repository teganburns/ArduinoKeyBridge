#ifndef ARDUINO_KEY_BRIDGE_NEOPIXEL_H
#define ARDUINO_KEY_BRIDGE_NEOPIXEL_H

#include <Adafruit_NeoPixel.h>
#include "ArduinoKeyBridgeLogger.h"

// Color constants for easy reference
struct NeoPixelColor {
    uint8_t r, g, b;
    constexpr NeoPixelColor(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0) : r(r_), g(g_), b(b_) {}
};

namespace NeoPixelColors {
    static constexpr NeoPixelColor RED(255, 0, 0);
    static constexpr NeoPixelColor GREEN(0, 255, 0);
    static constexpr NeoPixelColor BLUE(0, 0, 255);
    static constexpr NeoPixelColor YELLOW(255, 255, 0);
    static constexpr NeoPixelColor MAGENTA(255, 0, 255);
    static constexpr NeoPixelColor CYAN(0, 255, 255);
    static constexpr NeoPixelColor WHITE(255, 255, 255);
    static constexpr NeoPixelColor BLACK(0, 0, 0);
}

class ArduinoKeyBridgeNeoPixel {
public:
    static ArduinoKeyBridgeNeoPixel& getInstance();
    
    void begin(uint8_t pin = 6, uint16_t numPixels = 24);
    void setColor(const NeoPixelColor& color);
    void setBrightness(uint8_t brightness);
    void clear();
    void show();
    
    // Status indicator methods
    void setStatusIdle();
    void setStatusBusy();
    void setStatusError();
    void setStatusSuccess();

    // Setup progress indicator (0.0 to 1.0)
    void showSetupProgress(float progress);

    // Non-blocking animation update - call this in loop()
    void update();

    // New method to set individual pixel colors using NeoPixelColor constants
    void setPixelColors(const NeoPixelColor& pixel0, const NeoPixelColor& pixel1,
                       const NeoPixelColor& pixel2, const NeoPixelColor& pixel3,
                       const NeoPixelColor& pixel4, const NeoPixelColor& pixel5,
                       const NeoPixelColor& pixel6, const NeoPixelColor& pixel7);

    // Roll a color through all pixels
    void rollColor(int delayMs);

    // Overload setColor to take NeoPixelColor
    void setColor(uint8_t r, uint8_t g, uint8_t b);

private:
    ArduinoKeyBridgeNeoPixel() = default;
    ~ArduinoKeyBridgeNeoPixel() = default;
    ArduinoKeyBridgeNeoPixel(const ArduinoKeyBridgeNeoPixel&) = delete;
    ArduinoKeyBridgeNeoPixel& operator=(const ArduinoKeyBridgeNeoPixel&) = delete;

    // Helper method for alternating colors
    void setAlternatingColors(uint8_t r1, uint8_t g1, uint8_t b1, 
                            uint8_t r2, uint8_t g2, uint8_t b2);

    Adafruit_NeoPixel* pixels = nullptr;
    bool initialized = false;

    // Animation state
    unsigned long lastUpdate = 0;
    static const unsigned long UPDATE_INTERVAL = 1000; // Update every 1 second
    bool currentPattern = false; // false = first color, true = second color
    
    // Current colors
    struct Color {
        uint8_t r, g, b;
        Color(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0) : r(r_), g(g_), b(b_) {}
    };
    Color color1, color2;

    // Store the currently active color for rollColor
    Color activeColor;
};

#endif 