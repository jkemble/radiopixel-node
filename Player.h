#include <radiopixel_protocol.h>
#include "Pattern.h"

class Player
{
public:
    Player()
        : packet( NULL ), pattern( NULL ), patternId( HatPacket::Gradient ), 
          lastUpdate( 0 ), speed( 35 )
    {
    }

    void SetPacket( HatPacket *packet );

    //! update to the next pattern in the sequence if needed
    // returns true if pattern changed, ie need to transmit
    bool UpdatePattern( time_t now, Stripper *strip );

    //! update the strip with the current pattern if needed
    void UpdateStrip( time_t now, Stripper *strip );

    HatPacket *packet;
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

    void AddStep( int duration, HatPacket *packet )
    {
        if ( stepCount < 10 )
        {
            steps[ stepCount ].duration = duration;
            steps[ stepCount ].packet = packet;
            stepCount++;
        }
    }

    class Step
    {
    public:
        Step( int _duration = 0, HatPacket *_packet = NULL )
            : duration( _duration ), packet( _packet )
        {
        }
            
        int duration; // seconds
        HatPacket *packet;
    };

    Step steps[ 10 ];
    int stepCount;
};

 */
