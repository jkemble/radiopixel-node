#include <Arduino.h>
#include <stdarg.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <radiopixel_protocol.h>
#include "Player.h"
#include "Sequence.h"

// In order to support different harware, look in the ./Board/Board.h There you
// will see the base class for hardware. New boards should have their own .h/.cpp 
// files accordingly and relevant information there. 

// There are 3 board configurations so far. Look in ./Board to see the ones 
// already configured. Find examples commented below and what pin the LED lights
// are attached to.

// For the BlueFruit_Feather_Board
#include "Board/BlueFruit_Feather_Board.h"
Feather32u4BlueFruit board = Feather32u4BlueFruit( );
const int STRIP_PIN = A5;

// For the 1.3 Custom Made Board
// #include "Board/Custom_Board_1_3.h"
// CustomBoardVer1_3 board = CustomBoardVer1_3( );
// const int STRIP_PIN = A5;

// For the 1.0 Custom Made Board
// #include "Board/Custom_Board_1_0.h"
// CustomBoardVer1_0 board = CustomBoardVer1_0( );
// const int STRIP_PIN = 8;

// Strip Configuration setup
const int STRIP_LENGTH = 144; // 135

Stripper strip( STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800 );

// Pattern Classes
Player player;
IdleSequence idle;
AlertSequence alert;
RandomSequence randm;

RadioPixel::Command recvPacket; // last packet received
PacketSequence recvSequence( &recvPacket );
time_t lastTransmit = 0; // time of last retransmit
const time_t TRANSMIT_MS = 1000;

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

void Blink( byte pin, int ms )
{
  pinMode( pin, OUTPUT );
  digitalWrite( pin, HIGH );
  delay( ms );
  digitalWrite( pin, LOW );
}

void setup()
{
  Blink( board.GetLedPin(), 50 );

  // serial/debug
  Serial.begin( board.GetSerialBaud() );
  Serial.println( "RadioPixel" );
  pinMode( board.GetLedPin(), OUTPUT );

  // strip
  strip.begin();
  strip.setBrightness( 20 );

  // progress
  strip.setPixelColor( 0, GREEN );
  strip.show();

  // Set up Buttons if applicable to the board.
  board.SetupButtons();
  strip.setPixelColor( 1, GREEN );
  strip.show();

  // progress
  strip.setPixelColor( 2, GREEN );
  strip.show();

  // Set up bluetooth if applicable
  board.SetupBluetooth();

  //progress
  strip.setPixelColor( 3, GREEN );
  strip.show();

  // Set up FM Radio if applicable
  board.SetupFMRadio();

  // progress
  strip.setPixelColor( 4, GREEN );
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

  // Checks first to see if there is a Packet in from the App ( typically Bluetooth )
  // If it is, we become the controller. Otherwise, check the FM Radio. If something is
  // received via the radio, we are no longer the controller. Show the Sequence we receive.

  // Finally, check the buttons if the board defines them. Become the controller and show
  // the Sequence sepecified.

  // In all cases, if we are the controller, make sure we send the Look out to any other
  // nodes listening via the FM Radio.

  if ( board.CheckRecieveAppPacket(recvPacket) )
  {
    player.SetSequence( &recvSequence );
  }
  else if ( board.CheckRecieveFMPacket(recvPacket) )
  {
    player.SetSequence(&recvSequence);
  }

  // If the board doesn't support buttons, Type NONE will be returned.
  Sequence::Type seqType = board.CheckButtons();
  if ( seqType != Sequence::Type::NONE)
  {
    Sequence *seq = NULL;
    if ( seqType == Sequence::Type::ALERT)
    {
      seq = &alert;
    }
    else if (seqType == Sequence::Type::RANDOM)
    {
      seq = &randm;
    }

    // apply new sequence if applicable
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
  if ( board.IsController() && ( now - lastTransmit ) > TRANSMIT_MS )
  {
    RadioPixel::Command xmitCommand;
    if ( player.GetCommand( &xmitCommand ) )
    {
      board.TransmitFMPacket( xmitCommand );
    }

    lastTransmit = now;
  }
}


