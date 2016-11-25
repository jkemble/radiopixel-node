#include "Stripper.h"
#include "Gradient.h"

typedef unsigned long time_t;

class Pattern
{
public:
    Pattern( );
    
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) { return 40; }

    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, const uint32_t *colors, const uint8_t *levels, time_t offset );

    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, time_t offset ) { Loop( strip, offset ); }

    // restarting after a loop expired, but not first call 
    virtual void Loop( Stripper *strip, time_t offset ) { Update( strip, offset ); }

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) { }

    // opposite of Init, releases memory
    virtual void Cleanup( Stripper *strip ) { }

    // returns color
    uint32_t color( int index ) const
    {
        return m_color[ index % 3 ];
    }

    // returns level
    uint8_t level( int index ) const
    {
        return m_level[ index % 3 ];
    }

protected:
    uint32_t m_color[ 3 ];
    uint8_t m_level[ 3 ];
};

// Flash the entire strip
class FlashPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

// Rainbow!
class RainbowPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class SparklePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Loop( Stripper *strip, time_t offset );
};

class MiniSparklePattern : public SparklePattern
{
public:
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class MiniTwinklePattern : public Pattern
{
public:
    MiniTwinklePattern();

    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, time_t offset );
    
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // restarting after a loop expired, but not first call 
    virtual void Loop( Stripper *strip, time_t offset );
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );

    // opposite of Init, releases memory
    virtual void Cleanup( Stripper *strip );
    
protected:
    static const int MAX_PIXELS = 256;
    
    struct Pixel
    {
        Pixel( ) 
            : lit( false ), color( 0 ), start( 0 )
        {
        }
        
        bool lit;
        uint8_t color;
        time_t start;
    };
    
    Pixel *m_pixels;
    int m_lit;
    time_t m_lastOffset;
};

class MarchPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class WipePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class GradientPattern : public Pattern
{
public:
    GradientPattern( );
    
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, time_t offset );

    // restarting after a loop expired, but not first call 
    virtual void Loop( Stripper *strip, time_t offset );
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
    
    // opposite of Init, releases memory
    virtual void Cleanup( Stripper *strip );
    
private:
    Gradient grad;
    uint8_t *mp1, *mp2;
};

// Strobe the entire strip
class StrobePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );

    time_t m_lastOffset;
};

class FixedPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class CandyCanePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

class TestPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip );

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset );
};

