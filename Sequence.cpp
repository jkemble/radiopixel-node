#include "Sequence.h"

const uint8_t FULL = 127;

const Step idleStep =  
    {     0,     20,  35, RadioPixel::Command::Gradient, RED, WHITE, GREEN, 17 };

const Step alertSteps[] =
{
    {  4000,   FULL, 100, RadioPixel::Command::Flash, YELLOW, YELLOW, YELLOW, 255 },
    { 60000,   FULL,  40, RadioPixel::Command::March, YELLOW, YELLOW, YELLOW, 34 },
    { 60000,   FULL, 100, RadioPixel::Command::MiniTwinkle, YELLOW, Stripper::Color( 255, 255, 64 ), YELLOW, 75 },
    {     0, FULL/2,  75, RadioPixel::Command::Gradient, YELLOW, Stripper::Color( 255, 255, 64 ), YELLOW, 75 },
    {  4000,   FULL, 100, RadioPixel::Command::Flash, RED, RED, RED, 255 },
    { 60000,   FULL,  40, RadioPixel::Command::March, RED, RED, RED, 34 },
    { 60000,   FULL, 100, RadioPixel::Command::MiniTwinkle, RED, Stripper::Color( 255, 64, 64 ), RED, 75 },
    {     0, FULL/2,  75, RadioPixel::Command::Gradient, RED, Stripper::Color( 255, 64, 64 ), RED, 75 },
    idleStep
};

const Step randomSteps[] = 
{
    { 30000, FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, YELLOW, 160 }, // rwy twinkle
    { 30000, FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, GREEN, 160 }, // rwg twinkle
    { 30000, FULL,  35, RadioPixel::Command::Gradient, RED, WHITE, RED, 17 }, // rwr subtle
    { 30000, FULL,  75, RadioPixel::Command::Gradient, BLUE, Stripper::Color( 128, 128, 255 ), BLUE, 75 }, // blue smooth
    { 30000, FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, BLUE, 160 }, // rwb
    { 30000, FULL/2, 65, RadioPixel::Command::CandyCane, RED, WHITE, GREEN, 255 }, // rwg candy
    { 30000, FULL/2, 100, RadioPixel::Command::CandyCane, RED, WHITE, RED, 255 }, // rwr candy
    { 30000, FULL, 100, RadioPixel::Command::Fixed, RED, WHITE, GREEN, 255 }, // rwg tree
    { 30000, FULL, 127, RadioPixel::Command::March, RED, WHITE, GREEN, 8 }, // rwg march
    { 30000, FULL, 127, RadioPixel::Command::Wipe, RED, WHITE, GREEN, 8 }, // rwg wipe
    { 30000, FULL, 255, RadioPixel::Command::MiniSparkle, RED, WHITE, GREEN, 9 }, // rwg flicker
//    { 30000, FULL, 100, RadioPixel::Command::MiniTwinkle, CYAN, MAGENTA, YELLOW, 128 }, // cga
//    { 30000, FULL, 100, RadioPixel::Command::Rainbow, WHITE, WHITE, WHITE, 255 }, //  rainbow
//    { 30000, FULL, 128, RadioPixel::Command::Strobe, WHITE, WHITE, WHITE, 255 }, // strobe
    idleStep
};

IdleSequence::IdleSequence( )
    : StepSequence( &idleStep, 1 )
{
}

AlertSequence::AlertSequence( )
    : StepSequence( alertSteps, sizeof( alertSteps ) / sizeof( alertSteps[ 0 ] ) )
{
}

RandomSequence::RandomSequence( )
    : StepSequence( randomSteps, sizeof( randomSteps ) / sizeof( randomSteps[ 0 ] ) )
{
}

int RandomSequence::Reset( )
{
    return random( GetStepCount( ) - 1 );
}

int RandomSequence::Advance( int step, bool timed )
{
    if ( timed )
    {
        // timeout - go to idle
        return GetStepCount( ) - 1;
    }
    else
    {
        // manual advance - restart
        return Reset( );
    }
}

