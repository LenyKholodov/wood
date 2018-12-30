#define  DEBUG
#include <fixed\fixed.h>
#include <iostream.h>
#include <time.h>

const    int    TOTAL = 10000000;

void     main ()
{
  Fixed data [4] = {int2fixed(1),int2fixed(23),int2fixed(17),int2fixed(56)};

  float start    = clock ();

  for (int i=0;i<TOTAL;i++)
    data [i&3] = div (data [(i+1)&3],data [(i+2)&3]) + FX_ONE;

  float end      = clock ();

  cout<<"FPS: "<<float(TOTAL*CLK_TCK)/(end-start)<<endl;
}