#define  DEBUG
#include <iostream.h>
#include <math.h>
#include <fixed\fixed.h>
#include <time.h>

const    int    TOTAL = 10000000;

void     main ()
{
  float test [4];

  float start = clock ();

  for (int i=0;i<TOTAL;i++)
    test [i&3] = test [(i+1)&3] * test [(i-1)&3];
//    test [i&3] = cos (test [(i+1)&3]) * sin (test [(i-1)&3]);  

  float end = clock ();

  cout<<"FPS: "<<float(CLK_TCK*TOTAL)/(end-start)<<endl;

  Fixed test1 [4];

  start = clock ();

  for (i=0;i<TOTAL;i++)
    test1 [i&3] = mul (test1 [(i+1)&3],test1 [(i-1)&3]);
//    test1 [i&3] = mul (cos ((Angle)test1 [(i+1)&3]),sin ((Angle)test1 [(i-1)&3]));

  end = clock ();

  cout<<"FPS: "<<float(CLK_TCK*TOTAL)/(end-start)<<endl;
}
