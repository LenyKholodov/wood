#define DEBUG
#include <iostream.h>

float    x0  = 0,
         y0  = 0,
         x1  = 1,       
         y1  = 1,
         x2  = 1,
         y2  = -1;  

float    xt1 = 15,
         yt1 = 5,
         xt2 = -5,
         yt2 = 0;

float    a1,b1,c1,a2,b2,c2;

void     main ()
{
  float dx = x1 - x0,
        dy = y1 - y0;

  a1 = -dy;
  b1 = dx;
  c1 = -(dx*y0-dy*x0);

  dx = x0 - x2,
  dy = y0 - y2;

  a2 = -dy;
  b2 = dx;
  c2 = -(dx*y0-dy*x0);

  cout<<(a1*xt1+b1*yt1+c1)<<endl;
  cout<<(a2*xt1+b2*yt1+c2)<<endl;
}