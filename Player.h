#include <radiopixel_protocol.h>
#include "Pattern.h"

const int MAX_STEPS = 10;

class Seq
{
public:
    //! advance to the next set of steps
    virtual void Advance( ) { }
    
    //! number of steps
    virtual int GetStepCount( ) = 0;
    
    //! time (in ms) to stay in this step
    virtual unsigned long GetDuration( int step ) = 0;

    virtual int GetCommand( int step ) = 0;
    virtual int GetBrightness( int step ) = 0;
    virtual int GetSpeed( int step ) = 0;
    virtual int GetPatternId( int step ) = 0;
    virtual uint32_t GetColor( int step, int color ) = 0;
    virtual uint8_t GetLevel( int step, int level ) = 0;
};

class Player
{
public:
    Player()
        : stepCount( 0 ), step( 0 ), 
          pattern( NULL ), patternId( RadioPixel::Command::Gradient ), 
          lastUpdate( 0 ), speed( 35 )
    {
    }

    //! Replace all commands with a single command
    void SetCommand( RadioPixel::Command *packet );

    //! remove all commands
    void ClearCommands( );

    //! append a command
    void AddCommand( unsigned long duration, RadioPixel::Command *command );

    //! Returns the current command
    RadioPixel::Command *GetCommand( );

    //! update to the next pattern in the sequence if needed
    // returns true if pattern changed, ie need to transmit
    bool UpdatePattern( time_t now, Stripper *strip );

    //! update the strip with the current pattern if needed
    void UpdateStrip( time_t now, Stripper *strip );

    class Step
    {
    public:
        Step( unsigned long _duration = 0, RadioPixel::Command *_command = NULL )
            : duration( _duration ), command( _command )
        {
        }
            
        unsigned long duration; // milliseconds
        RadioPixel::Command *command;
    };

    Step steps[ MAX_STEPS ];
    int stepCount;
    int step; // the current step index
    unsigned long stepTime; // time we started the current step
    
    Pattern *pattern;
    uint8_t patternId;    
    time_t lastUpdate;
    uint8_t speed;
};

const time_t FRAME_MS = 1000 / 125;

/*
class Sequence
{
public:
    Sequence()
        : stepCount( 0 )
    {
    }

};

 */
