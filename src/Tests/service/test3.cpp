#include <kernel.h>

void     test ()
{
  cout<<"I'm here!"<<endl;
}

void     main ()
{
  K_Init (1,test);
  SrvMakeCurrent (D_Load ("test3.map"));

  int* x = (int*) NULL;

  *x     = 0;

  D_Done ();
  K_Done ();
}