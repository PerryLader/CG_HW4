#include "ColorGC.h"
#include <algorithm>
//argb



ColorGC::ColorGC() : color(0xFF0000FF) {} // Default: blue, fully opaque
ColorGC::ColorGC(uint8_t r, uint8_t g, uint8_t b, uint8_t a )
    : color((a << 24) | (r << 16) | (g << 8) | b) {
}


ColorGC::ColorGC(uint32_t argb) : color(argb) {}

// Getters for individual components
uint8_t ColorGC::getRed() const { return (color >> 16) & 0xFF; }
uint8_t ColorGC::getGreen()const { return (color >> 8) & 0xFF; }
uint8_t ColorGC::getBlue()const { return color  & 0xFF; }
uint8_t ColorGC::getAlpha() const { return (color>>24) & 0xFF; }

// Setters for individual components
void ColorGC::setRed(uint8_t r) { color = (color & 0xFF00FFFF) | (r << 16); }
void ColorGC::setGreen(uint8_t g) { color = (color & 0xFFFF00FF ) | (g << 8); }
void ColorGC::setBlue(uint8_t b) { color = (color & 0xFFFFFF00 ) | b ; }
void ColorGC::setAlpha(uint8_t a) { color = (color & 0x00FFFFFF) | (a<<24); }

// Get the entire RGBA value
uint32_t ColorGC::getARGB()  const{ return color; }
uint32_t ColorGC::getRGBA()  const {
    return (color << 8) | (color >> (24));
}
    

// Set the entire RGBA value
void ColorGC::setARGB(uint32_t rgba) { color = rgba; }

// Convert to string in hexadecimal format
std::string ColorGC::toHex()  const{
    std::ostringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << color;
    return ss.str();
}
ColorGC ColorGC::defaultColor() {
    return ColorGC(ColorGC::defaultRed, ColorGC::defaultGreen, ColorGC::defaultBlue, ColorGC::defaultAlpha);
 }



static uint8_t clamp(uint32_t x) {
    if (x < 0) return 0;
    if (x > 255) return 255;
    return x;
}

ColorGC ColorGC::mixTwoColors(const ColorGC& a, const ColorGC& b)
{
    uint32_t red = ((int)a.getRed() + b.getRed()) / 2;
    uint32_t green = ((int)a.getGreen() + b.getGreen()) / 2;
    uint32_t blue = ((int)a.getBlue() + b.getBlue()) / 2;
    uint32_t alpha = ((int)a.getAlpha() + b.getAlpha()) / 2;
    return ColorGC(red, green, blue, alpha);
}

ColorGC ColorGC::operator+(const ColorGC& other) const
{
    uint32_t red= this->getRed()+other.getRed();
    uint32_t green=this->getGreen() + other.getGreen();
    uint32_t blue=this->getBlue() + other.getBlue();
    uint32_t alpha = this->getAlpha() + other.getAlpha();

    
    return ColorGC(clamp(red),
        clamp(green),
        clamp(blue),
        clamp(alpha));
}

ColorGC ColorGC::operator-(const ColorGC& other) const
{
    uint32_t red = this->getRed() - other.getRed();
    uint32_t green = this->getGreen() - other.getGreen();
    uint32_t blue = this->getBlue() - other.getBlue();
    uint32_t alpha = this->getAlpha() - other.getAlpha();


    return ColorGC(clamp(red),
        clamp(green),
        clamp(blue),
        clamp(alpha));
}

ColorGC ColorGC::operator*(const float scalar) const
{
    uint32_t red = this->getRed() * scalar;
    uint32_t green = this->getGreen() * scalar;
    uint32_t blue = this->getBlue() * scalar;
    uint32_t alpha = this->getAlpha() * scalar;


    return ColorGC(clamp(red),
        clamp(green),
        clamp(blue),
        clamp(alpha));
}

ColorGC ColorGC::operator*(const ColorGC& other) const
{
    float red = ((float)this->getRed()/255) * ((float)other.getRed() / 255);
    float green = ((float)this->getGreen() / 255) * ((float)other.getGreen() / 255);
    float blue = ((float)this->getBlue() / 255) * ((float)other.getBlue() / 255);
    float alpha = ((float)this->getAlpha() / 255) * ((float)other.getAlpha() / 255);


    return ColorGC(clamp(red*255),
        clamp(green * 255),
        clamp(blue * 255),
        clamp(alpha * 255));
}




