#include "Stripper.h"


Stripper::Stripper( uint16_t pixels, uint8_t pin, uint8_t type )
    : Adafruit_NeoPixel( pixels, pin, type )
{
}

void Stripper::setAllColor( uint32_t color)
{
    for ( uint16_t i = 0; i < numPixels( ); i++ ) 
    {
        setPixelColor( i, color );
    }
}

void Stripper::setAllFade( uint8_t v )
{
    for ( uint16_t i = 0; i < numPixels( ); i++ ) 
    {
        setPixelColor( i, ColorFade( getPixelColor( i ), v ) );
    }
}

uint32_t Stripper::ColorFade( uint32_t c, uint8_t v )
{
    uint8_t
        r = (uint8_t)(c >> 16),
        g = (uint8_t)((c & 0xFF00) >> 8),
        b = (uint8_t)(c & 0xFF);
    return Color( fade( 0, r, v ), fade( 0, g, v ), fade( 0, b, v ) );    
}

uint32_t Stripper::ColorBlend( uint32_t color1, uint32_t color2, uint8_t v )
{
    uint8_t
        r1 = (uint8_t)(color1 >> 16),
        g1 = (uint8_t)((color1 & 0xFF00) >> 8),
        b1 = (uint8_t)(color1 & 0xFF),
        r2 = (uint8_t)(color2 >> 16),
        g2 = (uint8_t)((color2 & 0xFF00) >> 8),
        b2 = (uint8_t)(color2 & 0xFF);
    return Color( fade( r1, r2, v ), fade( g1, g2, v ), fade( b1, b2, v ) );    
}

uint32_t Stripper::ColorRandom( )
{
    return ColorWheel( random( 255 ) );
}

uint32_t Stripper::ColorWheel( byte WheelPos )
{
    if(WheelPos < 85) 
    {
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    else if(WheelPos < 170) 
    {
        WheelPos -= 85;
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } 
    else 
    {
        WheelPos -= 170;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}


