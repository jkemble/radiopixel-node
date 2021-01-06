#ifndef _STRIPPER_H
#define _STRIPPER_H

#include <Adafruit_NeoPixel.h>


// utilities

// generic fader
inline uint32_t fade( uint32_t low, uint32_t high, uint8_t v )
{
    return ( high - low ) * v / 255 + low;
}


// adds convenience methods to base strip class
class Stripper : public Adafruit_NeoPixel
{
public:
    Stripper( uint16_t pixels, uint8_t pin, uint8_t type );
    
    // set all pixels to a color
    void setAllColor( uint32_t color );

    // decrease all pixels intensity by value (0-255)
    void setAllFade( uint8_t value );  // --> Function is not used ?

    // color tools

    // decrease intensity by value (0-255)
    static uint32_t ColorFade( uint32_t color, uint8_t value );

    // crossfade between colors
    static uint32_t ColorBlend( uint32_t color1, uint32_t color2, uint8_t value );

    // random color
    static uint32_t ColorRandom( );

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    static uint32_t ColorWheel( byte WheelPos );
};


// colors
const uint32_t BLACK    = 0x000000;
const uint32_t RED      = 0xff0000;
const uint32_t GREEN    = 0x00ff00;
const uint32_t BLUE     = 0x0000ff;
const uint32_t CYAN     = 0x00ffff;
const uint32_t MAGENTA  = 0xff00ff;
const uint32_t YELLOW   = 0xffff00;
const uint32_t WHITE    = 0xffffff;


#endif

