#include <iostream.h>
#include <time.h>
#include <string.h>
#include <kernel.h>

void     main ()
{
  K_FastInit (NULL);

  const int size = 1;
  const int TOTAL = 10000000;

  char* temp = new char [size];

  MemBinFile file (temp,size);

  if (!temp)
  {
    cout<<"No enough memory"<<endl;
    return;
  }

  double start = clock ();

  for (int i=0;i<TOTAL;i++)
    file.write (temp,size,0);

  double end = clock ();

  cout<<"FPS:\t"<<double (CLK_TCK)*double(size)/(end-start)*double (TOTAL)<<endl;
  cout<<"FPS (at trans):\t"<<double (TOTAL)/(end-start)*double (CLK_TCK)<<endl;

  delete [] temp;
}