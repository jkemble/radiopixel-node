#include "radiopixel_protocol.h"

namespace RadioPixel
{

Command::Command( )
    : command( HC_NONE ), brightness( 255 ), speed( 100 ), 
    pattern( 0 ) //, countdown( 0 )
{
    color[ 0 ] = 0xff0000;
    color[ 1 ] = 0xffffff;
    color[ 2 ] = 0x0000ff;

    level[ 0 ] = level[ 1 ] = level[ 2 ] = 255;
}

Command::Command( int _brightness, int _speed, int _pattern,
    uint32_t color1, uint32_t color2, uint32_t color3,
    uint8_t level1, uint8_t level2, uint8_t level3 )
    : command( HC_PATTERN ), brightness( _brightness ), speed( _speed ),
    pattern( _pattern ) //, countdown( 0 )
{
    color[ 0 ] = color1;
    color[ 1 ] = color2;
    color[ 2 ] = color3;

    level[ 0 ] = level1;
    level[ 1 ] = level2;
    level[ 2 ] = level3;
}

} // namespace
