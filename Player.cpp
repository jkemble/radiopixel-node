#include "Player.h"


void Player::SetCommand( RadioPixel::Command *command )
{
    steps[ 0 ].duration = 0;
    steps[ 0 ].command = command;
    stepCount = 1;
    step = 0;
    stepTime = millis( );
}

void Player::ClearCommands( )
{
    stepCount = 0;
    step = 0;
    stepTime = millis( );
}

void Player::AddCommand( unsigned long duration, RadioPixel::Command *command )
{
    if ( stepCount < MAX_STEPS )
    {
        steps[ stepCount ].duration = duration;
        steps[ stepCount ].command = command;
        stepCount++;
    }
}

RadioPixel::Command *Player::GetCommand( )
{
    //--> get the complete pattern + brightness instead of just the pattern
    // this doesnt handle brightness being set, then a new pattern coming up - brightness would be stomped!
    return steps[ step ].command;
}

bool Player::UpdatePattern( time_t now, Stripper *strip )
{
    // move to the next command if needed
    if ( ( steps[ step ].duration > 0 ) &&
        ( now >= ( stepTime + steps[ step ].duration ) ) )
    {
        step = ( step + 1 ) % stepCount;
        stepTime = now;
    }
  
    // update the pattern to match the command
    bool changed = false;
    RadioPixel::Command *command = steps[ step ].command;
    if ( command )
    {
        switch ( command->command )
        {
        case HC_PATTERN:
            if ( !pattern ||
                command->pattern != patternId ||
                command->color[ 0 ] != pattern->color( 0 ) ||
                command->color[ 1 ] != pattern->color( 1 ) ||
                command->color[ 2 ] != pattern->color( 2 ) ||
                command->level[ 0 ] != pattern->level( 0 ) ||
                command->level[ 1 ] != pattern->level( 1 ) ||
                command->level[ 2 ] != pattern->level( 2 ) )
            {
                delete pattern;
                patternId = command->pattern;
                pattern = CreatePattern( command->pattern );
                time_t duration( pattern->GetDuration( strip ) );
                time_t offset = ( now * speed / 100 ) % duration;
                pattern->Init( strip, command->color, command->level, offset );
                strip->show();
                changed = true;
            }
            // fall through!
            
        case HC_CONTROL:
            strip->setBrightness( command->brightness );
            speed = command->speed;
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

