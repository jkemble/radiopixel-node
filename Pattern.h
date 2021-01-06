#pragma once

#include "Stripper.h"
#include "Gradient.h"

typedef unsigned long time_t;

class Pattern
{
public:
    Pattern( );

    virtual ~Pattern() {}
    
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


// Pattern factory
Pattern *CreatePattern( uint8_t pattern );


// Flash the entire strip
class FlashPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

// Rainbow!
class RainbowPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class SparklePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Loop( Stripper *strip, time_t offset ) override;
};

class MiniSparklePattern : public SparklePattern
{
public:
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class MiniTwinklePattern : public Pattern
{
public:
    MiniTwinklePattern();
    
    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, time_t offset ) override;

    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;

protected:
    time_t delta( time_t previous, time_t next, time_t duration );

    time_t m_lastDim;
    time_t m_lastLit;
};

class MarchPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class WipePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class GradientPattern : public Pattern
{
public:
    GradientPattern( );

    ~GradientPattern( );
    
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // assume nothing, setup all pixels
    virtual void Init( Stripper *strip, time_t offset ) override;

    // restarting after a loop expired, but not first call 
    virtual void Loop( Stripper *strip, time_t offset ) override;
    
    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
        
private:
    Gradient grad;
    uint8_t *mp1, *mp2;
};

// Strobe the entire strip
class StrobePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;

    time_t m_lastOffset;
};

class FixedPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class CandyCanePattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class TestPattern : public Pattern
{
public:
    // returns loop duration, time offset never goes above this
    virtual time_t GetDuration( Stripper *strip ) override;

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;
};

class DiagnosticPattern : public Pattern
{
public:
    DiagnosticPattern( int code = 0 ) 
        : m_code( code ) { }

    // update pixels as needed
    virtual void Update( Stripper *strip, time_t offset ) override;

    int m_code;
};

