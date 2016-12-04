#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <radiopixel_protocol.h>
#include "Player.h"
#include "Button.h"


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

Player player;

// radio

//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
RFM69 radio;
bool controller = false; // did we initiate the latest command?
RadioPixel::Command recvPacket; // last packet received
RadioPixel::Command xmitPacket; // packet to retransmit
time_t lastTransmit = 0; // time of last retransmit
const time_t TRANSMIT_MS = 1000;

// buttons (board v1.3 and above)

const int BUTTON1_PIN = 4;
const int BUTTON2_PIN = 5;
Button button1( BUTTON1_PIN ), button2( BUTTON2_PIN );

// flash (board v1.3 and above)

const int FLASH_PIN = 8;

// reusable patterns

RadioPixel::Command idle( 20, 35, RadioPixel::Command::Gradient, RED, WHITE, GREEN, 17, 128, 128 );
RadioPixel::Command rwy( 128, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, YELLOW, 160 );
RadioPixel::Command rwg( 128, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, GREEN, 160 );
RadioPixel::Command rgb( 128, 160, RadioPixel::Command::MiniTwinkle, RED, GREEN, BLUE, 160 );


void setup() 
{
    // serial/debug
    Serial.begin( SERIAL_BAUD );
    Serial.println( "HatNode" );
    pinMode( LED_PIN, OUTPUT );

    // strip
    strip.begin();
    strip.setBrightness( 20 );

    // progress
    strip.setPixelColor( 0, WHITE );
    strip.show();

    // buttons
    digitalWrite( BUTTON1_PIN, HIGH );
    digitalWrite( BUTTON2_PIN, HIGH );

    // progress
    strip.setPixelColor( 1, WHITE );
    strip.show();    

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
    strip.show();

    randomSeed(analogRead(0));

    // start idle pattern
    player.SetPacket( &idle );
    
    Serial.println( "setup complete");
}

void loop( )
{
    time_t now = millis( );
    
    // if we receive a command from serial then become the controller
    if ( Serial.available( ) >= sizeof recvPacket )
    {
        // read the packet
        Serial.readBytes( ( char *)&recvPacket, sizeof recvPacket );
        
        // take control
        controller = true;
        player.SetPacket( &recvPacket );
    }

    // if we receive a command from the radio then release control
    if ( radio.receiveDone( ) && 
        radio.DATALEN == sizeof( RadioPixel::Command ) )
    {
        // read the packet        
        memcpy( &recvPacket, ( void * )radio.DATA, radio.DATALEN );

        // release control
        controller = false;
        player.SetPacket( &recvPacket );
    }

    // check the buttons
    button1.update( );
    button2.update( );
    if ( !button1.down && !button2.down )
    {
        if ( ( button1.duration( ) >= 2500 ) && ( button2.duration( ) >= 2500 ) )
        {
            controller = !controller;
            player.SetPacket( &rgb );
        }
        else if ( button1.duration( ) )
        {
            player.SetPacket( &rwy );
        }
        else if ( button2.duration( ) )
        {
            player.SetPacket( &rwg );
        }
        button1.clear( );
        button2.clear( );
    }
    
    // run the player
    if ( player.UpdatePattern( now, &strip ) )
    {
        lastTransmit = 0;
    }
    player.UpdateStrip( now, &strip );

    // retransmit if we're the controller and haven't sent for a while
    if ( controller && ( now - lastTransmit ) > TRANSMIT_MS )
    {
        if ( player.packet )
        {
            //--> get the complete pattern + brightness instead of just the pattern
            // this doesnt handle brightness being set, then a new pattern coming up - brightness would be stomped!
            radio.send( HN_NODEID, player.packet, sizeof *( player.packet ) );
        }
        lastTransmit = now;
    }
}

void Blink( byte pin, int ms )
{
    pinMode( pin, OUTPUT );
    digitalWrite( pin, HIGH );
    delay( ms );
    digitalWrite( pin, LOW );
}

