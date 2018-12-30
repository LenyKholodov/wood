#include <math3d.h>
#include <graph\grtypes.h>

void     main ()
{
  plane_t p = {2,6,-3,33};  

  normalize (p);

  cout<<(p.a*-1.0+p.b*2+p.c*-2.0+p.d)<<endl;
}