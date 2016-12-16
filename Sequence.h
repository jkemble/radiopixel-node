#pragma once

#include <avr/pgmspace.h>
#include <radiopixel_protocol.h>
#include "Pattern.h"


class Sequence
{
public:
    // start at the first set of steps
    virtual int Reset( ) { return 0; }
    //! advance to the next step
    virtual int Advance( int step, bool timed = false ) { return ( step + 1 ) % GetStepCount( ); }
    
    //! number of steps
    virtual int GetStepCount( ) = 0;
    
    //! time (in ms) to stay in this step
    virtual unsigned long GetDuration( int step ) = 0;

    virtual int GetCommand( int step ) { return HC_PATTERN; }
    virtual int GetBrightness( int step ) = 0;
    virtual int GetSpeed( int step ) = 0;
    virtual int GetPatternId( int step ) = 0;
    virtual uint32_t GetColors( int step, int color ) = 0;
    virtual uint8_t GetLevels( int step, int level ) = 0;
};

class PacketSequence : public Sequence
{
public:
    PacketSequence( RadioPixel::Command *_packet ) : packet( _packet ) { }

    virtual int GetStepCount( ) { return 1; }
    virtual unsigned long GetDuration( int step ) { return 0; }
    virtual int GetCommand( int step ) { return packet->command; }
    virtual int GetBrightness( int step ) { return packet->brightness; }
    virtual int GetSpeed( int step ) { return packet->speed; }
    virtual int GetPatternId( int step ) { return packet->pattern; }
    virtual uint32_t GetColors( int step, int color ) { return packet->color[ color ]; }
    virtual uint8_t GetLevels( int step, int level ) { return packet->level[ level ]; }

    RadioPixel::Command *packet;
};


#pragma pack( push, 1 )
struct Step
{
    unsigned long duration;
    uint8_t brightness;
    uint8_t speed;
    uint8_t pattern;
    uint32_t colors[ 3 ];
    uint8_t level;
};
#pragma pack( pop )


class StepSequence : public Sequence
{
public:
    StepSequence( const Step *_steps, int _stepCount ) : steps( _steps ), stepCount( _stepCount ) { }
    virtual int GetStepCount( ) { return stepCount; }
    virtual unsigned long GetDuration( int step ) { return steps[ step ].duration; }
    virtual int GetBrightness( int step ) { return steps[ step ].brightness; }
    virtual int GetSpeed( int step ) { return steps[ step ].speed; }
    virtual int GetPatternId( int step ) { return steps[ step ].pattern; }
    virtual uint32_t GetColors( int step, int color ) { return steps[ step ].colors[ color ]; }
    virtual uint8_t GetLevels( int step, int level ) { return ( level == 0 ) ? steps[ step ].level : 0; }

    const Step *steps;
    int stepCount;
};

class IdleSequence : public StepSequence
{
public:
    IdleSequence( );
};

class AlertSequence : public StepSequence
{
public:
    AlertSequence( );
};

class RandomSequence : public StepSequence
{
public:
    RandomSequence( );
    
    virtual int Reset( );
    virtual int Advance( int step, bool timed = false );
};

