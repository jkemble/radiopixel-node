#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <radiopixel_protocol.h>
#include "Stripper.h"
#include "Gradient.h"
#include "Pattern.h"


// board rev (x 10)
#define BOARD_REV 13 

// serial/debug

const long SERIAL_BAUD = 9600; //for bluetooth, was 115200;
const int LED_PIN = 9;

// strip

const int STRIP_LENGTH = 92; //135
#if BOARD_REV <= 12
const int STRIP_PIN = 8;
#else
const int STRIP_PIN = A5;
#endif
Stripper strip( STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800 );

// patterns

Pattern *pattern = NULL;
uint8_t patternId = HatPacket::Gradient;
const time_t FRAME_MS = 1000 / 125;
time_t lastUpdate = 0;
uint8_t speed = 35;

// radio

//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
RFM69 radio;
bool controller = false; // did we initiate the latest command?
HatPacket xmitPacket; // packet to retransmit
time_t lastTransmit = 0; // time of last retransmit
const time_t TRANSMIT_MS = 1000;

// buttons (board v1.3 and above)

const int BUTTON1_PIN = 4;
const int BUTTON2_PIN = 5;
struct button
{
    button( int _pin )
        : pin( _pin ), down( false ), pressed( 0 ), released( 0 ), handled( true )
    {
    }
    
    int pin;
    bool down;
    time_t pressed, released;
    bool handled;
};
button button1( BUTTON1_PIN ), button2( BUTTON2_PIN );

// flash (board v1.3 and above)

const int FLASH_PIN = 8;

// macros

class MacroStep : public HatPacket
{
public:
    MacroStep( int _duration, int _brightness, int _speed, uint8_t level1, int _pattern,
        uint32_t color1, uint32_t color2, uint32_t color3,
        uint8_t level2 = 255, uint8_t level3 = 255 )
        : HatPacket( _brightness, _speed, _pattern, color1, color2, color3, level1, level2, level3 ),
        duration( _duration )
    {
    }
        
    int duration; // seconds
};

class Macro
{
public:
    Macro()
        : stepCount( 0 )
    {
    }
    
    void AddStep( MacroStep *step)
    {
        if ( stepCount < 10 )
        {
            steps[ stepCount ] = step;        
            stepCount++;
        }
    }
    
    MacroStep *steps[ 10 ];
    int stepCount;
};

/*
const PROGMEM MacroStep idle( 0, 20, 35, 17, HatPacket::Gradient, RED, WHITE, GREEN );
const PROGMEM MacroStep hi( 0, 128, 160, 160, HatPacket::MiniTwinkle, RED, WHITE, YELLOW );
*/

void setup() 
{
    // serial/debug
    Serial.begin( SERIAL_BAUD );
    Serial.println( "HatNode" );
    pinMode( LED_PIN, OUTPUT );

    // buttons
    digitalWrite( BUTTON1_PIN, HIGH );
    digitalWrite( BUTTON2_PIN, HIGH );

    // strip
    strip.begin();
    strip.setBrightness( 20 );

    // progress
    strip.setPixelColor( 0, WHITE );
    strip.show(); // Initialize all pixels to 'off'

    // progress
    strip.setPixelColor( 1, WHITE );
    strip.show(); // Initialize all pixels to 'off'    

    // radio
    delay( 10 );
    radio.initialize( HN_FREQUENCY, HN_NODEID, HN_NETWORKID );
#ifdef IS_RFM69HW
    radio.setHighPower(); //only for RFM69HW!
#endif
    radio.encrypt( HN_KEY );
    radio.promiscuous( false );

    // progress
    strip.setPixelColor( 2, WHITE );
    strip.show(); // Initialize all pixels to 'off'

    randomSeed(analogRead(0));

    // start idle pattern
    pattern = CreatePattern( patternId );
    time_t duration( pattern->GetDuration( &strip ) );
    uint32_t colors[3] = { RED, WHITE, GREEN };
    uint8_t slow[ 3 ] = { 17, 128, 128 };
    pattern->Init( &strip, colors, slow, 0 ); //idle.color, idle.level, 0 );
    strip.show();
    
    Serial.println( "setup complete");
}

void loop( )
{
    time_t now = millis( );
    HatPacket recvPacket;
    
    // if we receive a command from serial then become the controller
    if ( Serial.available( ) >= sizeof recvPacket )
    {
        // read the packet
        Serial.readBytes( ( char *)&recvPacket, sizeof recvPacket );
        
        // take control
        controller = true;
        lastTransmit = 0;
    }

    // if we receive a command from the radio then release control
    if ( radio.receiveDone( ) && 
        radio.DATALEN == sizeof( HatPacket ) )
    {
        // read the packet        
        memcpy( &recvPacket, ( void * )radio.DATA, radio.DATALEN );

        // release control
        controller = false;
    }

    // check the buttons
    updateButton( &button1 );
    updateButton( &button2 );
    if ( !button1.handled && 
        ( button1.released - button1.pressed ) >= 50 )
    {
        button1.handled = true;
/*        
        recvPacket = hi;
*/        
        recvPacket.command = HC_PATTERN;
        recvPacket.brightness = 128;
        recvPacket.speed = 160;
        recvPacket.pattern = 0;
        recvPacket.color[ 0 ] = 0xff0000;
        recvPacket.color[ 1 ] = 0xffffff;
        recvPacket.color[ 2 ] = 0xffff00;
        recvPacket.level[ 0 ] = 160;
//*/        
    }
    else if ( !button2.handled && 
        ( button2.released - button2.pressed ) >= 50 )
    {
        button2.handled = true;

        recvPacket.command = HC_PATTERN;
        recvPacket.brightness = 128;
        recvPacket.speed = 160;
        recvPacket.pattern = 0;
        recvPacket.color[ 0 ] = 0xff0000;
        recvPacket.color[ 1 ] = 0xffffff;
        recvPacket.color[ 2 ] = 0x00ff00;
        recvPacket.level[ 0 ] = 160;
    }

    // process the packet if something was received
    switch ( recvPacket.command )
    {
    case HC_PATTERN:
        if ( recvPacket.pattern != patternId ||
            recvPacket.color[ 0 ] != pattern->color( 0 ) ||
            recvPacket.color[ 1 ] != pattern->color( 1 ) ||
            recvPacket.color[ 2 ] != pattern->color( 2 ) ||
            recvPacket.level[ 0 ] != pattern->level( 0 ) ||
            recvPacket.level[ 1 ] != pattern->level( 1 ) ||
            recvPacket.level[ 2 ] != pattern->level( 2 ) )
        {
            delete pattern;
            patternId = recvPacket.pattern;
            pattern = CreatePattern( recvPacket.pattern );
            xmitPacket = recvPacket;
            time_t duration( pattern->GetDuration( &strip ) );
            time_t offset = ( now * speed / 100 ) % duration;
            pattern->Init( &strip, recvPacket.color, recvPacket.level, offset );
            strip.show();
        }
        // fall through!
        
    case HC_CONTROL:
        strip.setBrightness( recvPacket.brightness );
        speed = recvPacket.speed;
        xmitPacket.brightness = recvPacket.brightness;
        xmitPacket.speed = recvPacket.speed;
        break;

    case HC_NONE:
    default:
        break;
    }
    
    // run the pattern
    if ( ( now - lastUpdate ) > FRAME_MS )
    {
        time_t duration( pattern->GetDuration( &strip ) );
        time_t offset = ( now * speed / 100 ) % duration;
        if ( ( ( now * speed / 100 ) / duration ) != ( ( lastUpdate * speed / 100 ) / duration ) )
        {
            pattern->Loop( &strip, offset );
        }
        else
        {
            pattern->Update( &strip, offset );
        }
        strip.show();
        
        lastUpdate = now;
    }

    // retransmit if we're the controller and haven't sent for a while
    if ( controller && ( now - lastTransmit ) > TRANSMIT_MS )
    {
        radio.send( HN_NODEID, &xmitPacket, sizeof xmitPacket );

        lastTransmit = now;
    }
}

/*
void loop1()
{
    // Test smear()
    //flash(0x000040);
    grad.randomize(0,128);
    strip.show();
    delay(40);
    for (int c = 0; c < 100; c++)
    {
        int p = random(1, STRIP_LENGTH - 1), l = grad.getPixel(p);
        while (l < 251)
        {
            l+=4;
            grad.setPixel(p, l);
            grad.setPixel(p + 1, l);
            grad.setPixel(p - 1, l);
            grad.peturb( -16, 16);
            strip.show();
            delay(40);
        }

        for (int i = 0; i < 10 ; i++) 
        {
            grad.fade();
            grad.peturb( -16, 16);
            //grad.smear();
            strip.show();
            delay(40);
        }
    }
}
*/

void Blink( byte pin, int ms )
{
    pinMode( pin, OUTPUT );
    digitalWrite( pin, HIGH );
    delay( ms );
    digitalWrite( pin, LOW );
}

void updateButton( struct button *b )
{
    bool down = ( digitalRead( b->pin ) == LOW );
    if ( down != b->down )
    {
        b->down = down;
        if ( down )
        {
            b->pressed = millis();
        }
        else
        {
            b->released = millis();
            b->handled = false;
        }
    }
}

