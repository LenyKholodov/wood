#include <iostream.h>
#include <time.h>
#include <string.h>

void     main ()
{
  const int size = 1024*1024*256;
  char* temp = new char [size];

  if (!temp)
  {
    cout<<"No enough memory"<<endl;
    return;
  }

  double start = clock ();

  memcpy (temp,temp,size);

  double end = clock ();

  cout<<"FPS:\t"<<double (CLK_TCK)*double(size)/(end-start)<<endl;

  delete [] temp;
}