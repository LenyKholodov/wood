#include <fixed\tables.h>
#include "table.dat"

const Fixed* _sin    = (Fixed*)__sin;

Fixed  sin(Angle angle)  {  return _sin[angle>>3]; }
Fixed  cos(Angle angle)  {  return sin(ANGLE_90-angle);      }
