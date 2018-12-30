#include <iostream.h>
#include <math.h>

float    x = 12, y = 14;
float    len = 10;


void     main ()
{
  float len1  = sqrt (x*x + y*y);
  float k     = len/len1;  
  x          *= k;
  y          *= k;

  cout<<sqrt (x*x + y*y)<<endl;
}

