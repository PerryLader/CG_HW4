#pragma once
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>


class ColorGC {
private:
    uint32_t color; // Internal representation: 0xAARRGGBB
  
public:
    // Constructors
    ColorGC();
    ColorGC(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    

    ColorGC(uint32_t rgba);

    // Getters for individual components
    uint8_t getRed() const;
    uint8_t getGreen() const;
    uint8_t getBlue() const;
    uint8_t getAlpha() const;

    // Setters for individual components
    void setRed(uint8_t r);
    void setGreen(uint8_t g);
    void setBlue(uint8_t b);
    void setAlpha(uint8_t a);

    uint32_t getARGB() const;
    uint32_t getRGBA() const;
    // Set the entire RGBA value
    void setARGB(uint32_t rgba);

    // Convert to string in hexadecimal format
    std::string toHex() const;
    static ColorGC defaultColor();
    static const uint8_t defaultRed = 0;
    static const uint8_t defaultGreen = 100;
    static const uint8_t defaultBlue = 100;
    static const uint8_t defaultAlpha = 255;
    static ColorGC mixTwoColors(const ColorGC& a, const ColorGC& b);
    //oprators
    ColorGC operator+(const ColorGC& other) const;
    ColorGC operator-(const ColorGC& other) const;
    ColorGC operator*(const float scalar) const;
    ColorGC operator*(const ColorGC& other) const;

};

