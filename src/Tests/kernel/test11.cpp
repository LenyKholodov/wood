#include <iostream.h>
#include <kernel.h>
#include <time.h>

const int total = 100000000;

void     main ()
{
  K_FastInit (NULL);

  static threadptr_t<int> iPtr = 0;
//  static int iPtr = 0;

  float start = clock ();

  for (int i=0;i<total;i++)
    iPtr = iPtr + 1;

  float end   = clock ();

  cout<<"FPS:\t"<<float (total)/(end-start)*float (CLK_TCK)<<endl;
}