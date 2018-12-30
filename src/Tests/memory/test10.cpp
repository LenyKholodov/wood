#include <default.h>

void     main ()
{
  char* x = new char [1024*1024*128*5];

  dout<<(void*)x<<endl;

  float start = clock ();

  memset (x,0,1024*1024*128*3);

  float end = clock ();

  cout<<(end-start)/float (CLK_TCK)<<endl;

//  start = clock ();

//  memset (x,0,1024*1024*128*3);

//  end = clock ();

//  cout<<(end-start)/float (CLK_TCK)<<endl;
}