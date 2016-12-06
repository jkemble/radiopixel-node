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
enum Sequence
{
    Idle,
    Warn,
    Exit,
    Other
};
Sequence sequence;

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

const int FULL = 255;
// idle
RadioPixel::Command idle( 20, 35, RadioPixel::Command::Gradient, RED, WHITE, GREEN, 17, 128, 128 );
// warn
RadioPixel::Command warn1( FULL, 100, RadioPixel::Command::Flash, YELLOW, YELLOW, YELLOW, 255 );
RadioPixel::Command warn2( FULL,  40, RadioPixel::Command::March, YELLOW, YELLOW, YELLOW, 34 );
RadioPixel::Command warn3( FULL, 100, RadioPixel::Command::MiniTwinkle, YELLOW, Stripper::Color( 255, 255, 64 ), YELLOW, 75 );
RadioPixel::Command warn4( FULL/2, 75, RadioPixel::Command::Gradient, YELLOW, Stripper::Color( 255, 255, 64 ), YELLOW, 75 );
// exit
RadioPixel::Command exit1( FULL, 100, RadioPixel::Command::Flash, RED, RED, RED, 255 );
RadioPixel::Command exit2( FULL,  40, RadioPixel::Command::March, RED, RED, RED, 34 );
RadioPixel::Command exit3( FULL, 100, RadioPixel::Command::MiniTwinkle, RED, Stripper::Color( 255, 64, 64 ), RED, 75 );
RadioPixel::Command exit4( FULL/2, 75, RadioPixel::Command::Gradient, RED, Stripper::Color( 255, 64, 64 ), RED, 75 );
// random
RadioPixel::Command rwyTwinkle( FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, YELLOW, 160 );
RadioPixel::Command rwgTwinkle( FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, GREEN, 160 );
RadioPixel::Command rwrSubtle( FULL, 35, RadioPixel::Command::Gradient, RED, WHITE, RED, 17 );
RadioPixel::Command blueSmooth( FULL, 75, RadioPixel::Command::Gradient, BLUE, Stripper::Color( 128, 128, 255 ), BLUE, 75 );
/*
RadioPixel::Command rwb( FULL, 160, RadioPixel::Command::MiniTwinkle, RED, WHITE, BLUE, 160 );
RadioPixel::Command rwgCandy( FULL/2, 65, RadioPixel::Command::CandyCane, RED, WHITE, GREEN, 255 );
RadioPixel::Command rwrCandy( FULL/2, 100, RadioPixel::Command::CandyCane, RED, WHITE, RED, 255 );
RadioPixel::Command rwgTree( FULL, 100, RadioPixel::Command::Fixed, RED, WHITE, GREEN, 255 );
RadioPixel::Command rwgMarch( FULL, 127, RadioPixel::Command::March, RED, WHITE, GREEN, 8 );
RadioPixel::Command rwgWipe( FULL, 127, RadioPixel::Command::Wipe, RED, WHITE, GREEN, 8 );
RadioPixel::Command rwgFlicker( FULL, 255, RadioPixel::Command::MiniSparkle, RED, WHITE, GREEN, 9 );
RadioPixel::Command cga( FULL, 100, RadioPixel::Command::MiniTwinkle, CYAN, MAGENTA, YELLOW, 128 );
RadioPixel::Command rainbow( FULL, 100, RadioPixel::Command::Rainbow, WHITE, WHITE, WHITE, 255 );
RadioPixel::Command strobe( FULL, 128, RadioPixel::Command::Strobe, WHITE, WHITE, WHITE, 255 );
*/


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
    player.SetCommand( &idle );
    sequence = Idle;
    
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
        player.SetCommand( &recvPacket );
        sequence = Other;
    }

    // if we receive a command from the radio then release control
    if ( radio.receiveDone( ) && 
        radio.DATALEN == sizeof( RadioPixel::Command ) )
    {
        // read the packet        
        memcpy( &recvPacket, ( void * )radio.DATA, radio.DATALEN );

        // release control
        controller = false;
        player.SetCommand( &recvPacket );
        sequence = Other;
    }

    // check the buttons
    button1.update( );
    button2.update( );
    if ( !button1.down && !button2.down )
    {
        if ( ( button1.duration( ) >= 2500 ) && ( button2.duration( ) >= 2500 ) )
        {
            controller = !controller;
            //player.SetCommand( &rgb );
        }
        else if ( button1.duration( ) )
        {
            // warning - exit - idle
            player.ClearCommands( );
            switch ( sequence )
            {
            default: // everything goes to warning
                player.AddCommand(  4000, &warn1 );
                player.AddCommand( 60000, &warn2 );
                player.AddCommand( 60000, &warn3 );
                player.AddCommand(     0, &warn4 );
                sequence = Warn;
                break;

            case Warn: // warning goes to exit
                player.AddCommand(  4000, &exit1 );
                player.AddCommand( 60000, &exit2 );
                player.AddCommand( 60000, &exit3 );
                player.AddCommand(     0, &exit4 );
                sequence = Exit;
                break;

            case Exit: // exit goes to idle
                player.SetCommand( &idle );
                sequence = Idle;
                break;
            }
        }
        else if ( button2.duration( ) )
        {
            player.ClearCommands( );
            int r = random( 14 );
            switch ( r )
            {
            default:
            case 0: player.AddCommand( 30000, &rwyTwinkle ); break;
            case 1: player.AddCommand( 30000, &rwgTwinkle ); break;
            case 2: player.AddCommand( 30000, &rwrSubtle ); break;
/*            
            case 3: player.AddCommand( 30000, &blueSmooth ); break;
            case 4: player.AddCommand( 30000, &rwb ); break;
            case 5: player.AddCommand( 30000, &rwgCandy ); break;
            case 6: player.AddCommand( 30000, &rwrCandy ); break;
            case 7: player.AddCommand( 30000, &rwgTree ); break;
            case 8: player.AddCommand( 30000, &rwgMarch ); break;
            case 9: player.AddCommand( 30000, &rwgWipe ); break;
            case 10: player.AddCommand( 30000, &rwgFlicker ); break;
            case 11: player.AddCommand( 30000, &cga ); break;
            case 12: player.AddCommand( 30000, &rainbow ); break;
            case 13: player.AddCommand( 30000, &strobe ); break;
*/            
            }
            player.AddCommand( 0, &idle );
            sequence = Other;
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
        RadioPixel::Command *command = player.GetCommand( );
        if ( command )
        {
            radio.send( HN_NODEID, command, sizeof *command );
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

