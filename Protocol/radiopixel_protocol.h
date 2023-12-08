#pragma once

#include <stdint.h>
#include <string.h>

//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define HN_FREQUENCY   RF69_433MHZ
//#define HN_FREQUENCY   RF69_868MHZ
#define HN_FREQUENCY     RF69_915MHZ

// encryption
#define HN_KEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!

// network addresses
#define HN_NETWORKID     100  // the same on all nodes that talk to each other
#define HN_CONTROLLERID  2    // controller/transmitter
#define HN_NODEID        1    // hat/receiver

// commands
#define HC_NONE         0 // no command
#define HC_CONTROL	1 // brightness and speed only
#define HC_PATTERN      2 // switch to a pattern

#pragma pack( push, 1 )

namespace RadioPixel
{

class Command
{
public:
    Command( );

    Command( int _brightness, int _speed, int _pattern,
        uint32_t color1, uint32_t color2, uint32_t color3,
        uint8_t level1 = 255, uint8_t level2 = 255, uint8_t level3 = 255 );

    bool operator==( const Command& rhs ) const
    {
        return memcmp( this, &rhs, sizeof( Command ) ) == 0;
    }

    bool operator!=( const Command& rhs ) const
    {
        return !( *this == rhs );
    }

    // HC_XXX
    uint8_t command;

    // brightness, 0 (off) - 255 (maximum)
    uint8_t brightness;

    // speed in percent, 0 (stopped/slowest) - 255 (2.55x speedup)
    uint8_t speed;

    // patterns
    enum Pattern
    {
        MiniTwinkle,
        MiniSparkle,
        Sparkle,
        Rainbow,
        Flash,
        March,
        Wipe,
        Gradient,
        Fixed,
        Strobe,
        CandyCane
    };

    // pattern code
    uint8_t pattern;

    // number of milliseconds until execution
    // negative if execution already started
    //int32_t countdown;

    // colors
    uint32_t color[ 3 ];

    // levels
    uint8_t level[ 3 ];
};

} // namespace

#pragma pack( pop )

