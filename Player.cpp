#include "Player.h"


void Player::SetPacket( HatPacket *_packet )
{
    packet = _packet;
}

bool Player::UpdatePattern( time_t now, Stripper *strip )
{
    // update the pattern to match the command
    bool changed = false;
    if ( packet )
    {
        switch ( packet->command )
        {
        case HC_PATTERN:
            if ( !pattern ||
                packet->pattern != patternId ||
                packet->color[ 0 ] != pattern->color( 0 ) ||
                packet->color[ 1 ] != pattern->color( 1 ) ||
                packet->color[ 2 ] != pattern->color( 2 ) ||
                packet->level[ 0 ] != pattern->level( 0 ) ||
                packet->level[ 1 ] != pattern->level( 1 ) ||
                packet->level[ 2 ] != pattern->level( 2 ) )
            {
                delete pattern;
                patternId = packet->pattern;
                pattern = CreatePattern( packet->pattern );
                time_t duration( pattern->GetDuration( strip ) );
                time_t offset = ( now * speed / 100 ) % duration;
                pattern->Init( strip, packet->color, packet->level, offset );
                strip->show();
                changed = true;
            }
            // fall through!
            
        case HC_CONTROL:
            strip->setBrightness( packet->brightness );
            speed = packet->speed;
//            xmitPacket.brightness = recvPacket.brightness;
//            xmitPacket.speed = recvPacket.speed;
            changed = true;
            break;
    
        case HC_NONE:
        default:
            break;
        }
    }

    return changed;
}

void Player::UpdateStrip( time_t now, Stripper *strip )
{
    // update the strip if it's time
    if ( pattern && strip && ( ( now - lastUpdate ) > FRAME_MS ) )
    {
        time_t duration( pattern->GetDuration( strip ) );
        time_t offset = ( now * speed / 100 ) % duration;
        if ( ( ( now * speed / 100 ) / duration ) != 
            ( ( lastUpdate * speed / 100 ) / duration ) )
        {
            pattern->Loop( strip, offset );
        }
        else
        {
            pattern->Update( strip, offset );
        }
        strip->show();
        
        lastUpdate = now;
    }
}

