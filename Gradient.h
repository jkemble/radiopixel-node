
#ifndef _GRADIENT_H
#define _GRADIENT_H

#include "Stripper.h"


class Gradient
{
public:
  Gradient();

  struct Step
  {
    uint8_t pos;
    uint32_t color;
  };

  void clearSteps( );
  void addStep( uint8_t pos, uint32_t color );
  void setSteps( Step *st, uint8_t steps ); // --> Function is not used ?

  uint32_t getColor( uint8_t pos );

/*  
  void smear();
  void randomize();
  void randomize(int, int); // set low and high range
  void peturb(int, int); 
  void fade();
  void wipe(byte level);
*/
    
private:
  Step m_steps[ 10 ];
  byte m_stepCount;
};

#endif

