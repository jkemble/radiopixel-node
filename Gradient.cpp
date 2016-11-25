#include "Gradient.h"
#ifdef _DEBUG
#include <stdarg.h>

int freeRam()
{
    extern int __heapm_stepsart, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heapm_stepsart : (int) __brkval);
}

#endif

Gradient::Gradient()
    : m_stepCount( 0 )
{
}

void Gradient::clearSteps( )
{
    m_stepCount = 0;
}

void Gradient::addStep( uint8_t pos, uint32_t color )
{
    if ( m_stepCount < 10 )
    {
        m_steps[ m_stepCount ].pos = pos;
        m_steps[ m_stepCount ].color = color;
        m_stepCount++;
    }
}

void Gradient::setSteps( Step *steps, uint8_t stepCount )
{
    m_stepCount = min( stepCount, 10 );
    for ( int i = 0; i < m_stepCount; ++i )
    {
        m_steps[ i ] = steps[ i ];
    }
}

uint32_t Gradient::getColor( uint8_t pos )
{
    if ( pos <= m_steps[ 0 ].pos ) 
    {
        return m_steps[ 0 ].color;
    } 
    else if ( pos >= m_steps[ m_stepCount - 1 ].pos ) 
    {
        return m_steps[ m_stepCount - 1 ].color;
    }
    else 
    {
        int i = 0;
        while ( i < ( m_stepCount - 1 ) && 
            !( pos >= m_steps[ i ].pos && pos < m_steps[ i + 1 ].pos ) )
        {
            i++;
        }
        if ( i >= ( m_stepCount - 1 ) )
        {
            return 0;
        }
        if ( m_steps[ i + 1 ].pos == m_steps[ i ].pos )
        {
            return m_steps[ i ].color;
        }
        
        uint8_t f = ( pos - m_steps[ i ].pos ) * 255 / 
            ( m_steps[ i + 1 ].pos - m_steps[ i ].pos );
        return Stripper::ColorBlend( m_steps[ i ].color, m_steps[ i + 1 ].color, f );
    }
}

/*

void Gradient::smear()
{
    uint8_t prevVal, savePixel;
    uint16_t _numPixels = strip->numPixels();
    if (!strip || !strip->numPixels( ) || !_pixels)
    {
        return;
    }
    _pixels[0] = (uint8_t)((int)(_pixels[0] + _pixels[1])/3);
    prevVal = _pixels[0];
    for (int i=1; i < _numPixels - 1; i++)
    {
        savePixel = _pixels[i];
        _pixels[i] = (uint8_t)((int)(_pixels[i] + _pixels[i+1] + prevVal)/3);
        prevVal = savePixel;
    }
    _pixels[_numPixels-1] = (uint8_t)((int)(_pixels[_numPixels] + _pixels[prevVal])/3);
    updateStrip();
}

void Gradient::randomize()
{
    for (int i=0; i < strip->numPixels( ); i++)
    {
        _pixels[i] = random(255);
    }
    updateStrip();
}

void Gradient::randomize(int low, int high)
{
    for (int i=0; i < strip->numPixels( ); i++)
    {
        _pixels[i] = random(low, high);
    }
    updateStrip();
}

void Gradient::peturb(int low, int high)
{
    int temp;
    for (int i=0; i < strip->numPixels( ); i++)
    {
        temp = _pixels[i] + (random(low, high));
        if (temp < 0) temp = 0;
        if (temp > 255) temp = 255;
        _pixels[i] = temp;
    }
    updateStrip();
}

void Gradient::fade()
{
    for (int i=0; i < strip->numPixels( ); i++)
    {
        if (_pixels[i] < 4)
            _pixels[i] = 0;
        else
            _pixels[i] -= 4;
    }
    updateStrip();
}

void Gradient::wipe(byte level)
{
    for (int i=0; i < strip->numPixels( ); i++)
    {
        _pixels[i] = level;
    }
    updateStrip();
}

*/

