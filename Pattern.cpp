#include <radiopixel_protocol.h>
#include "Pattern.h"


Pattern::Pattern( )
{
    m_color[ 0 ] = 0xff0000;
    m_color[ 1 ] = 0xffffff;
    m_color[ 2 ] = 0x00ff00;
    
    m_level[ 0 ] = m_level[ 1 ] = m_level[ 2 ] = 0x80;
}

void Pattern::Init( Stripper *strip, const uint32_t *colors, const uint8_t *levels, time_t offset )
{
    m_color[ 0 ] = colors[ 0 ];
    m_color[ 1 ] = colors[ 1 ];
    m_color[ 2 ] = colors[ 2 ];
    
    m_level[ 0 ] = levels[ 0 ];
    m_level[ 1 ] = levels[ 1 ];
    m_level[ 2 ] = levels[ 2 ];

    Init( strip, offset );
}

//-------------------------------------------------------------

Pattern *CreatePattern( uint8_t pattern )
{
    switch ( pattern )
    {
    case HatPacket::MiniTwinkle:
        return new MiniTwinklePattern( );
    case HatPacket::MiniSparkle:
        return new MiniSparklePattern( );
    case HatPacket::Sparkle:
        return new SparklePattern( );
    case HatPacket::Rainbow:
        return new RainbowPattern( );
    case HatPacket::Flash:
        return new FlashPattern( );
    case HatPacket::March:
        return new MarchPattern( );
    case HatPacket::Wipe:
        return new WipePattern( );
    case HatPacket::Gradient:
        return new GradientPattern( );
    case HatPacket::Fixed:
        return new FixedPattern( );
    case HatPacket::Strobe:
        return new StrobePattern( );
    case HatPacket::CandyCane:
        return new CandyCanePattern( );
    default:
        return new DiagnosticPattern( 1 );
    }
}

//-------------------------------------------------------------

time_t FlashPattern::GetDuration( Stripper *strip )
{
    return 4000;
}

void FlashPattern::Update( Stripper *strip, time_t offset )
{
    uint16_t t = offset * 300 / GetDuration( strip );
    uint16_t o = t % 100;
    uint32_t col = color( t / 100 );
    if ( ( o >= 0 && o <= 10 ) || ( o >= 20 && o <= 30 ) )
    {
        strip->setAllColor( col );
    }
    else if ( o > 30 && o <= 60 )
    {
        uint8_t f = ( 60 - o ) * 255 / 30;
        strip->setAllColor( strip->ColorFade( col, f ) );      
    }
    else
    {
        strip->setAllColor( 0 );      
    }
}

//-------------------------------------------------------------

time_t RainbowPattern::GetDuration( Stripper *strip )
{
    return 2000;
}

void RainbowPattern::Update( Stripper *strip, time_t offset )
{
    for ( int i = 0; i < strip->numPixels( ); i++ ) 
    {
        uint8_t t = 255 - offset * 255 / GetDuration( strip );
        uint8_t p = i * 255 / strip->numPixels( );
        strip->setPixelColor( i, strip->ColorWheel( ( p + t ) % 255 ) );
    }
}

//-------------------------------------------------------------

time_t SparklePattern::GetDuration( Stripper *strip )
{
    return 100;
}

void SparklePattern::Loop( Stripper *strip, time_t offset )
{
    // strobe - new pixels each loop
    strip->setAllColor( 0 );
    for ( int c = fade( 1, strip->numPixels( ), m_level[ 0 ] ); c; c-- )
    {
        uint32_t col = color( random( 3 ) );
        if ( col == 0 )
            col = strip->ColorRandom( );
        strip->setPixelColor( random( strip->numPixels( ) ), col );
    }

    Update( strip, offset );
}

//-------------------------------------------------------------

void MiniSparklePattern::Update( Stripper *strip, time_t offset )
{
    // 25% duty cycle
    if ( offset > GetDuration( strip ) / 4 )
    {
        strip->setAllColor( 0 );
    }
}

//-------------------------------------------------------------

MiniTwinklePattern::MiniTwinklePattern()
{
    m_pixels = NULL;
}

void MiniTwinklePattern::Init( Stripper *strip, time_t offset )
{
    m_pixels = new Pixel[ strip->numPixels( ) ];
    m_lit = 0;
    m_lastOffset = 0;
}

time_t MiniTwinklePattern::GetDuration( Stripper *strip )
{
    return 1000;
}

void MiniTwinklePattern::Loop( Stripper *strip, time_t offset )
{
    m_lit = 0;
    Update( strip, offset );
}

void MiniTwinklePattern::Update( Stripper *strip, time_t offset )
{
    int duration( GetDuration( strip ) );

    // age out old pixels
    if ( m_pixels )
    {
        for ( int i = 0; i < strip->numPixels( ); ++i )
        {
            Pixel& pixel( m_pixels[ i ] );
            if ( pixel.lit )
            {
                if ( ( offset > m_lastOffset ) &&
                    ( m_lastOffset < pixel.start ) && ( pixel.start <= offset ) )
                {
                    pixel.lit = false;
                    m_lit--;
                }
                else if ( ( offset < m_lastOffset ) &&
                    ( ( m_lastOffset < pixel.start ) || ( pixel.start <= offset ) ) )
                {
                    pixel.lit = false;
                    m_lit--;
                }
            }
        }
    }
    
    // add any new pixels as needed
    int total( fade( 1, strip->numPixels( ), m_level[ 0 ] ) );
    int lit = offset * total / duration;
    while ( m_pixels && m_lit < lit )
    {
        int i = random( strip->numPixels( ) );
        Pixel& pixel( m_pixels[ i ] );
        pixel.lit = true;
        pixel.color = random( 3 );
        pixel.start = offset;
        m_lit++;
    }

    // update strip
    for ( int i = 0; i < strip->numPixels( ); ++i )
    {
        uint32_t col;
        if ( !m_pixels )
        {
            col = 0xff0000;
        }
        else
        {
            Pixel& pixel( m_pixels[ i ] );
            if ( pixel.lit )
            {
                col = color( pixel.color );
                time_t start = pixel.start;
                uint32_t age = ( offset >= start ) ? ( offset - start ) : ( duration + offset - start );
                uint8_t fade = 255 - age * 255 / duration;
                col = strip->ColorFade( col, fade );
            }
            else
            {
                col = 0;
            }
        }
        strip->setPixelColor( i, col );
    }

    m_lastOffset = offset;
}

MiniTwinklePattern::~MiniTwinklePattern( )
{
    delete [] m_pixels;
    m_pixels = NULL;
}

//-------------------------------------------------------------

time_t MarchPattern::GetDuration( Stripper *strip )
{
    return 1000;
}

void MarchPattern::Update( Stripper *strip, time_t offset )
{
    // a segment is one color, there are three segments in a loop

    // duration of a loop
    uint32_t duration = GetDuration( strip );
    // how far are we through all three segments
    uint32_t o = ( m_level[ 0 ] * 3 ) - ( offset * m_level[ 0 ] * 3 / duration );
    
    for ( int i = 0; i < strip->numPixels( ); i++ ) 
    {
        // fade level based on position within segment
        uint32_t e = ( i + o ) % m_level[ 0 ];
        if ( e > ( m_level[ 0 ] / 2 ) )
        {
            e = m_level[ 0 ] - e;
        }
        if ( e > ( m_level[ 0 ] / 4 ) )
        {
            e = e / 2;
        }
        else
        {
            e = 0;
        }
        uint8_t f = e * 255 / ( m_level[ 0 ] / 2 );

        // color based on segment
        uint32_t c = color( ( ( i + o ) / m_level[ 0 ] ) % 3 );

        strip->setPixelColor( i, strip->ColorFade( c, f ) );
    }
}

//-------------------------------------------------------------

time_t WipePattern::GetDuration( Stripper *strip )
{
    return 3000;
}

void WipePattern::Update( Stripper *strip, time_t offset )
{
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        int d = GetDuration( strip );
        int t = offset * ( strip->numPixels( ) * 3 ) / d;
        t = ( strip->numPixels( ) * 3 ) - t; // offset due to time
        int c = ( i + t ) / strip->numPixels( );
        int e = ( i + t ) % strip->numPixels( );
        uint8_t f = e * 255 / strip->numPixels( );
        f = ( f < 128 ) ? 0 : ( ( f - 128 ) * 2 );
        strip->setPixelColor( i, strip->ColorFade( color( c ), f ) );
    }
}

//-------------------------------------------------------------

Gradient::Step steps[ 4 ] = { { 0, 0x200000 }, { 32, 0xc01900 }, { 192, 0xff6010 }, { 255, 0x4040ff } };


GradientPattern::GradientPattern( )
{
    mp1 = mp2 = NULL;
}

time_t GradientPattern::GetDuration( Stripper *strip )
{
    return 1000;
}

void GradientPattern::Init( Stripper *strip, time_t offset )
{
    // setup gradient
    grad.clearSteps( );
    if ( m_level[ 0 ] > 6 && m_level[ 0 ] < 249 )
    {
        grad.addStep( 0, m_color[ 0 ] );
        grad.addStep( m_level[ 0 ] / 3, m_color[ 1 ] );
        grad.addStep( ( int )m_level[ 0 ] * 2 / 3, m_color[ 2 ] );
        grad.addStep( m_level[ 0 ], m_color[ 0 ] );
        grad.addStep( m_level[ 0 ] + 1, 0 );
        grad.addStep( 255, 0 );
    }
    else
    {
        grad.addStep( 0, m_color[ 0 ] );
        grad.addStep( 85, m_color[ 1 ] );
        grad.addStep( 170, m_color[ 2 ] );
        grad.addStep( 255, m_color[ 0 ] );
    }

    // setup maps
    mp1 = new uint8_t[ strip->numPixels( ) ];
    mp2 = new uint8_t[ strip->numPixels( ) ];
    if ( mp1 && mp2 )
    {
        for ( int i = 0; i < strip->numPixels( ); i++ )
        {
            mp1[ i ] = mp2[ i ] = i;
        }
    }
    Loop( strip, offset );
}

void GradientPattern::Loop( Stripper *strip, time_t offset )
{
    // create a new random map
    if ( mp1 && mp2 )
    {
        for ( int i = 0; i < strip->numPixels( ); i++ )
        {
            mp1[ i ] = mp2[ i ];
            mp2[ i ] = random( strip->numPixels( ) );
        }
    }
    Update( strip, offset );
}

void GradientPattern::Update( Stripper *strip, time_t offset )
{
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        uint32_t c1 = 0xff0000, c2 = 0xff0000;
        if ( mp1 && mp2 )
        {
            c1 = grad.getColor( ( int )mp1[ i ] * 255 / strip->numPixels( ) );
            c2 = grad.getColor( ( int )mp2[ i ] * 255 / strip->numPixels( ) );
        }
        strip->setPixelColor( i, Stripper::ColorBlend( c1, c2, offset * 255 / GetDuration( strip ) ) );
    }
}

GradientPattern::~GradientPattern( )
{
    delete [] mp1;
    mp1 = NULL;
    delete [] mp2;
    mp2 = NULL;
}

//-------------------------------------------------------------

time_t StrobePattern::GetDuration( Stripper *strip )
{
    return 750; // 4Hz at 100% spped, 10Hz at 250% speed
}

void StrobePattern::Update( Stripper *strip, time_t offset )
{
    time_t third( GetDuration( strip ) / 3 );
    if ( ( offset / third ) != ( m_lastOffset / third ) )
    {
        strip->setAllColor( color( offset / third ) );
    }
    else
    {
        strip->setAllColor( 0 );      
    }
    m_lastOffset = offset;
}

//-------------------------------------------------------------

time_t CandyCanePattern::GetDuration( Stripper *strip )
{
    return 200;
}

void CandyCanePattern::Update( Stripper *strip, time_t offset )
{
    int c = 0;
    if ( offset < ( GetDuration( strip ) / 2 ) )
        c = 1;
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        strip->setPixelColor( i, color( c + ( i % 2 ) ) );
    }
}

//-------------------------------------------------------------

time_t TestPattern::GetDuration( Stripper *strip )
{
    return 1000;
}

void TestPattern::Update( Stripper *strip, time_t offset )
{
/*  
    // test intensity scale
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        uint8_t f = i * 255 / strip->numPixels( );
        strip->setPixelColor( i, strip->ColorFade( 0xffffff, f ) );
    }
*/

    // test gradient
    Gradient grad;
    grad.clearSteps( );
    grad.addStep( 0, m_color[ 0 ] );
    grad.addStep( 85, m_color[ 1 ] );
    grad.addStep( 170, m_color[ 2 ] );
    grad.addStep( 255, m_color[ 0 ] );
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        strip->setPixelColor( i, grad.getColor( i * 255 / strip->numPixels( ) ) );
    }
}

//-------------------------------------------------------------

time_t FixedPattern::GetDuration( Stripper *strip )
{
    return 750;
}

void FixedPattern::Update( Stripper *strip, time_t offset )
{
    int step = 3 * offset / GetDuration( strip );
    uint32_t col( color( step ) );
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        strip->setPixelColor( i, ( i % 3 == step ) ? col : 0 );
    }
}

//-------------------------------------------------------------

void DiagnosticPattern::Update( Stripper *strip, time_t offset )
{
    const int space = 3;
    for ( int i = 0; i < strip->numPixels( ); i++ )
    {
        bool on( ( i % ( m_code + space ) ) < m_code );
        strip->setPixelColor( i, on ? WHITE : BLACK );
    }
}

