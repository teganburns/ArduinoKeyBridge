#include "ArduinoKeyBridgeNeoPixel.h"

ArduinoKeyBridgeNeoPixel& ArduinoKeyBridgeNeoPixel::getInstance() {
    static ArduinoKeyBridgeNeoPixel instance;
    return instance;
}

void ArduinoKeyBridgeNeoPixel::begin(uint8_t pin, uint16_t numPixels) {
    if (!initialized) {
        pixels = new Adafruit_NeoPixel(numPixels, pin, NEO_GRB + NEO_KHZ800);
        pixels->begin();
        pixels->setBrightness(255);  
        pixels->show();
        initialized = true;
    } else {
        ArduinoKeyBridgeLogger::getInstance().warning("NeoPixel", "Attempt to initialize already initialized NeoPixel");
    }
}

void ArduinoKeyBridgeNeoPixel::setColor(const NeoPixelColor& color) {
    setColor(color.r, color.g, color.b);
}

void ArduinoKeyBridgeNeoPixel::setColor(uint8_t r, uint8_t g, uint8_t b) {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to set color before initialization");
        return;
    }
    activeColor = Color(r, g, b);
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
    pixels->setBrightness(brightness);
    //pixels->setBrightness(10);
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::clear() {
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to clear before initialization");
        return;
    }
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
    setColor(0, 0, 255);  // Solid bright blue
}

void ArduinoKeyBridgeNeoPixel::setStatusBusy() {
    setColor(255, 165, 0);  // Solid bright orange
}

void ArduinoKeyBridgeNeoPixel::setStatusError() {
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
    
    // Calculate how many pixels should be blue
    int numPixels = pixels->numPixels() / 3; // 3 LEDs per pixel
    int bluePixels = round(progress * numPixels);
    
    // Set all pixels to red first
    for (int i = 0; i < numPixels; i++) {
        if (i < bluePixels) {
            // Progress pixels are white
            pixels->setPixelColor(i, pixels->Color(255, 255, 255));
        } else {
            // Remaining pixels are ded
            pixels->setPixelColor(i, pixels->Color(0, 0, 0));
        }
    }
    
    pixels->show();
}

void ArduinoKeyBridgeNeoPixel::setStatusSuccess() {
    setColor(0, 255, 0);  // Solid bright green
} 

void ArduinoKeyBridgeNeoPixel::rollColor(int delayMs) {
    // Use activeColor for rolling
    if (!initialized) {
        ArduinoKeyBridgeLogger::getInstance().error("NeoPixel", "Attempt to roll color before initialization");
        return;
    }
    uint32_t color = pixels->Color(activeColor.r, activeColor.g, activeColor.b);
    // Roll from bottom to top
    for(int i=0; i<pixels->numPixels(); i++) {
        for(int j=0; j<pixels->numPixels(); j++) pixels->setPixelColor(j,0);
        pixels->setPixelColor(i,color);
        pixels->show();
        delay(delayMs);
    }
    // Roll from top to bottom
    for(int i=pixels->numPixels()-1; i>=0; i--) {
        for(int j=0; j<pixels->numPixels(); j++) pixels->setPixelColor(j,0);
        pixels->setPixelColor(i,color);
        pixels->show();
        delay(delayMs);
    }
    // Clear all pixels at end
    for(int j=0; j<pixels->numPixels(); j++) pixels->setPixelColor(j,0);
    pixels->show();
}