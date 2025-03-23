#include "ArduinoKeyBridgeNeoPixel.h"

ArduinoKeyBridgeNeoPixel& ArduinoKeyBridgeNeoPixel::getInstance() {
    static ArduinoKeyBridgeNeoPixel instance;
    return instance;
}

void ArduinoKeyBridgeNeoPixel::begin(uint8_t pin, uint16_t numPixels) {
    if (!initialized) {
        String initMsg = "Initializing on pin " + String(pin) + " with " + String(numPixels) + " pixels";
        ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", initMsg.c_str());
        pixels = new Adafruit_NeoPixel(numPixels, pin, NEO_GRB + NEO_KHZ800);
        pixels->begin();
        pixels->setBrightness(20);  
        pixels->show();
        initialized = true;
        ArduinoKeyBridgeLogger::getInstance().info("NeoPixel", "NeoPixel initialized successfully");
    } else {
        ArduinoKeyBridgeLogger::getInstance().warning("NeoPixel", "Attempt to initialize already initialized NeoPixel");
    }
}

void ArduinoKeyBridgeNeoPixel::setColor(uint8_t r, uint8_t g, uint8_t b) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to set color before initialization");
        return;
    }
    String colorMsg = "Setting color to RGB(" + String(r) + "," + String(g) + "," + String(b) + ")";
    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", colorMsg.c_str());
    for(int i = 0; i < pixels->numPixels(); i++) {
        pixels->setPixelColor(i, pixels->Color(r, g, b));
    }
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::setAlternatingColors(uint8_t r1, uint8_t g1, uint8_t b1, 
                                                   uint8_t r2, uint8_t g2, uint8_t b2) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to set alternating colors before initialization");
        return;
    }
    
    // Store the colors for animation
    color1 = Color(r1, g1, b1);
    color2 = Color(r2, g2, b2);
    
    String colorMsg = "Setting alternating colors RGB(" + String(r1) + "," + String(g1) + "," + String(b1) + 
                     ") and RGB(" + String(r2) + "," + String(g2) + "," + String(b2) + ")";
    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", colorMsg.c_str());
    
    // Initial pattern
    currentPattern = false;
    for(int i = 0; i < pixels->numPixels(); i++) {
        if (i % 2 == 0) {
            pixels->setPixelColor(i, pixels->Color(r1, g1, b1));
        } else {
            pixels->setPixelColor(i, pixels->Color(r2, g2, b2));
        }
    }
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::update() {
    if (!initialized) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = currentTime;
        currentPattern = !currentPattern;
        
        for(int i = 0; i < pixels->numPixels(); i++) {
            if (i % 2 == currentPattern) {
                pixels->setPixelColor(i, pixels->Color(color1.r, color1.g, color1.b));
            } else {
                pixels->setPixelColor(i, pixels->Color(color2.r, color2.g, color2.b));
            }
        }
        pixels->show();
    }
}

void ArduinoKeyBridgeNeoPixel::setBrightness(uint8_t brightness) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to set brightness before initialization");
        return;
    }
    String brightnessMsg = "Setting brightness to " + String(brightness);
    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", brightnessMsg.c_str());
    pixels->setBrightness(brightness);
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::clear() {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to clear before initialization");
        return;
    }
    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", "Clearing all pixels");
    pixels->clear();
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::show() {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to show before initialization");
        return;
    }
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::setStatusIdle() {
    ArduinoKeyBridgeLogger::getInstance().info("NeoPixel", "Setting status: IDLE");
    setColor(0, 0, 255);  // Solid bright blue
}

void ArduinoKeyBridgeNeoPixel::setStatusBusy() {
    ArduinoKeyBridgeLogger::getInstance().info("NeoPixel", "Setting status: BUSY");
    setColor(255, 165, 0);  // Solid bright orange
}

void ArduinoKeyBridgeNeoPixel::setStatusError() {
    ArduinoKeyBridgeLogger::getInstance().info("NeoPixel", "Setting status: ERROR");
    setColor(255, 0, 0);  // Solid bright red
}

void ArduinoKeyBridgeNeoPixel::setPixelColors(const NeoPixelColor& pixel0, const NeoPixelColor& pixel1,
                                             const NeoPixelColor& pixel2, const NeoPixelColor& pixel3,
                                             const NeoPixelColor& pixel4, const NeoPixelColor& pixel5,
                                             const NeoPixelColor& pixel6, const NeoPixelColor& pixel7) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to set pixel colors before initialization");
        return;
    }

    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", "Setting individual pixel colors");
    
    // Array of pixel colors for easier iteration
    const NeoPixelColor* pixelColors[] = {
        &pixel0, &pixel1, &pixel2, &pixel3,
        &pixel4, &pixel5, &pixel6, &pixel7
    };

    // Set each pixel's color
    for (int i = 0; i < 8 && i < pixels->numPixels(); i++) {
        const NeoPixelColor* color = pixelColors[i];
        pixels->setPixelColor(i, pixels->Color(color->r, color->g, color->b));
    }

    // If there are more pixels, set them to off/black
    for (int i = 8; i < pixels->numPixels(); i++) {
        pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    }

    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::showSetupProgress(float progress) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to show setup progress before initialization");
        return;
    }

    // Clamp progress between 0 and 1
    progress = progress < 0.0f ? 0.0f : (progress > 1.0f ? 1.0f : progress);
    
    String progressMsg = "Setup progress: " + String(progress * 100) + "%";
    ArduinoKeyBridgeLogger::getInstance().debug("NeoPixel", progressMsg.c_str());

    // Calculate how many pixels should be blue
    int numPixels = pixels->numPixels() / 3; // 3 LEDs per pixel
    int bluePixels = round(progress * numPixels);
    
    // Set all pixels to red first
    for (int i = 0; i < numPixels; i++) {
        if (i < bluePixels) {
            // Progress pixels are green
            pixels->setPixelColor(i, pixels->Color(0, 255, 0));
        } else {
            // Remaining pixels are yellow
            pixels->setPixelColor(i, pixels->Color(255, 255, 0));
        }
    }
    
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::setStatusSuccess() {
    ArduinoKeyBridgeLogger::getInstance().info("NeoPixel", "Setting status: SUCCESS");
    setColor(0, 255, 0);  // Solid bright green
} 