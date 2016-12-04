
struct Button
{
    Button( int _pin )
        : pin( _pin ), down( false ), pressed( 0 ), released( 0 )
    {
    }

    void update( )
    {
        bool _down = ( digitalRead( pin ) == LOW );
        if ( _down != down )
        {
            down = _down;
            if ( _down )
            {
                pressed = millis();
            }
            else
            {
                released = millis();
            }
        }
    }

    //! returns duration of most recent button press, if not yet handled
    time_t duration( ) const
    {
        if ( down || ( pressed == 0 ) || ( released == 0 ) || 
            ( pressed >= released ) ||
            ( ( released - pressed ) < 50 ) ) // debounce
        {
            return 0;
        }
        return released - pressed;
    }

    //! indicates most recent press was now handled
    void clear( )
    {
        pressed = released = 0;
    }
    
    int pin;
    bool down;
    time_t pressed, released;
};

