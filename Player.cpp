#include "Player.h"


void Player::SetSequence( Sequence *_sequence )
{
    if ( sequence != _sequence )
    {
        sequence = _sequence;
        step = sequence->Reset( );
        stepTime = millis( );
    }
}

void Player::AdvanceSequence( )
{
    if ( !sequence)
    {
        return;
    }
    
    step = sequence->Advance( step );
    stepTime = millis( );
}

bool Player::GetCommand( RadioPixel::Command *command )
{
    command->command = HC_PATTERN;
    command->brightness = sequence->GetBrightness( step );
    command->speed = speed;
    command->pattern = patternId;
    command->color[ 0 ] = pattern->color( 0 );
    command->color[ 1 ] = pattern->color( 1 );
    command->color[ 2 ] = pattern->color( 2 );
    command->level[ 0 ] = pattern->level( 0 );
    command->level[ 1 ] = pattern->level( 1 );
    command->level[ 2 ] = pattern->level( 2 );
    return true;
}

bool Player::UpdatePattern( time_t now, Stripper *strip )
{
    if ( !sequence )
    {
        return false;
    }
    
    // move to the next command if needed
    if ( ( sequence->GetDuration( step ) > 0 ) &&
        ( now >= ( stepTime + sequence->GetDuration( step ) ) ) )
    {
        step = sequence->Advance( step, true );
        stepTime = now;
    }
  
    // update the pattern to match the command
    bool changed = false;

    switch ( sequence->GetCommand( step ) )
    {
    case HC_PATTERN:
        if ( !pattern ||
            sequence->GetPatternId( step ) != patternId ||
            sequence->GetColors( step, 0 ) != pattern->color( 0 ) ||
            sequence->GetColors( step, 1 ) != pattern->color( 1 ) ||
            sequence->GetColors( step, 2 ) != pattern->color( 2 ) ||
            sequence->GetLevels( step, 0 ) != pattern->level( 0 ) ||
            sequence->GetLevels( step, 1 ) != pattern->level( 1 ) ||
            sequence->GetLevels( step, 2 ) != pattern->level( 2 ) )
        {
#ifdef DEBUG          
            Serial.print( F("changing pattern to "));
            Serial.println( sequence->GetPatternId( step ));
#endif
            
            delete pattern;
            patternId = sequence->GetPatternId( step );
            pattern = CreatePattern( patternId );
            time_t duration( pattern->GetDuration( strip ) );
            time_t offset = ( now * speed / 100 ) % duration;
            uint32_t colors[ 3 ];
            colors[ 0 ] = sequence->GetColors( step, 0 );
            colors[ 1 ] = sequence->GetColors( step, 1 );
            colors[ 2 ] = sequence->GetColors( step, 2 );
            uint8_t levels[ 3 ];
            levels[ 0 ] = sequence->GetLevels( step, 0 );
            levels[ 1 ] = sequence->GetLevels( step, 1 );
            levels[ 2 ] = sequence->GetLevels( step, 2 );
            pattern->Init( strip, colors, levels, offset );
            strip->show();

            changed = true;

            strip->setBrightness( sequence->GetBrightness( step ) );
            speed = sequence->GetSpeed( step );
        }
        break;
        
    case HC_CONTROL:
        strip->setBrightness( sequence->GetBrightness( step ) );
        speed = sequence->GetSpeed( step );
        changed = true;
        break;

    case HC_NONE:
    default:
        break;
    }

    return changed;
}

void Player::UpdateStrip( time_t now, Stripper *strip )
{
    // update the strip if it's time
    if ( pattern && strip && ( ( now - lastUpdate ) > FRAME_MS ) )
    {
        time_t duration( pattern->GetDuration( strip ) );
        time_t offset = ( ( now - stepTime ) * speed / 100 ) % duration;
        unsigned long count = ( ( now - stepTime ) * speed / 100 ) / duration;
        unsigned long lastCount = ( ( lastUpdate - stepTime ) * speed / 100 ) / duration;
        if ( count != lastCount )
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

