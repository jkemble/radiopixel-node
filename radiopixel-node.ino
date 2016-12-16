#include <stdarg.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <radiopixel_protocol.h>
#include "Player.h"
#include "Sequence.h"
#include "Button.h"


//#define DEBUG

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
IdleSequence idle;
AlertSequence alert;
RandomSequence randm;

// radio

//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
RFM69 radio;
bool controller = false; // did we initiate the latest command?
RadioPixel::Command recvPacket; // last packet received
PacketSequence recvSequence( &recvPacket );
time_t lastTransmit = 0; // time of last retransmit
const time_t TRANSMIT_MS = 1000;

// buttons (board v1.3 and above)

const int BUTTON1_PIN = 4;
const int BUTTON2_PIN = 5;
Button button1( BUTTON1_PIN ), button2( BUTTON2_PIN );

// flash (board v1.3 and above)

const int FLASH_PIN = 8;

// todo:
// [ ] allow local control when no controller is transmitting
//    * need more formal concept of idle state (sequence return step -1 to reliquish control?)
//    * stop transmitting when in idle state
//    * disable local control if external control received within last 10 seconds
//    * idle command so all nodes know they are in idle?

int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() 
{
    Blink( LED_PIN, 50 );
  
    // serial/debug
    Serial.begin( SERIAL_BAUD );
    Serial.println( "RadioPixel" );
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
    player.SetSequence( &idle );
    
    Serial.println( "setup complete");
}

#ifdef DEBUG          
time_t lastUpdate = 0;
#endif

void loop( )
{
    time_t now = millis( );

#ifdef DEBUG          
    if ( now >= ( lastUpdate + 1000 ) )
    {
        Serial.print( F("free memory: " ) );
        Serial.print( freeRam( ) );
        Serial.println( F(" bytes" ) );
        lastUpdate = now;
    }
#endif

    // if we receive a command from serial then become the controller
    int avail = Serial.available( );
    if ( avail >= sizeof recvPacket )
    {
        // read the packet
#ifdef DEBUG          
        Serial.readBytes( ( char *)&recvPacket, sizeof recvPacket );
        Serial.print( F("received serial command, command "));
        Serial.print( recvPacket.command );
        Serial.print( F(", pattern "));
        Serial.println( recvPacket.pattern );
#endif        
        if ( avail > sizeof recvPacket )
        {
            int extra = 0;
            while ( Serial.available( ) )
            {
                Serial.read( );
                extra++;
            }
#ifdef DEBUG          
            Serial.print( F("serial overflow "));
            Serial.print( extra );
            Serial.println( F(" bytes"));
#endif
        }
          
        // take control
        controller = true;
        player.SetSequence( &recvSequence );
    }

    // if we receive a command from the radio then release control
    if ( radio.receiveDone( ) && 
        radio.DATALEN == sizeof( RadioPixel::Command ) )
    {
        // read the packet        
        memcpy( &recvPacket, ( void * )radio.DATA, radio.DATALEN );

        // release control
        controller = false;
        player.SetSequence( &recvSequence );
    }

    // check the buttons
    button1.update( );
    button2.update( );
    if ( !button1.down && !button2.down )
    {
        // determine what button was hit and the resulting sequence
        Sequence *seq = NULL;
        if ( ( button1.duration( ) >= 2500 ) && ( button2.duration( ) >= 2500 ) )
        {
            controller = !controller;
            // seq = controller ? &red : &green;
        }
        else if ( button1.duration( ) )
        {
            if ( controller )
            {
                seq = &alert;
            }
            else
            {
                seq = &randm;
            }
        }
        else if ( button2.duration( ) )
        {
            seq = &randm;
        }
        button1.clear( );
        button2.clear( );

        // apply new sequence
        if ( seq )
        {
            if ( player.GetSequence( ) != seq )
            {
                player.SetSequence( seq );
            }
            else
            {
                player.AdvanceSequence( );
            }
        }
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
        RadioPixel::Command xmitCommand;
        if ( player.GetCommand( &xmitCommand ) )
        {
            radio.send( HN_NODEID, &xmitCommand, sizeof xmitCommand );
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

