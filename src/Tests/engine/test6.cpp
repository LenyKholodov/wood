#include <kernel.h>
#include  <time.h>

const     int TOTAL = 1024*1024;

void      main ()
{
  float start = clock ();

  CritSect cs;

  for (int i=0;i<TOTAL;i++)
  {
    CSEntry trans (cs);    
  }

  float end  = clock ();

  cout<<"FPS:\t"<<float (TOTAL*CLK_TCK)/(end-start)<<endl;
}