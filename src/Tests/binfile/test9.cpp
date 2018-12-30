#include <kernel.h>

void     main ()
{
  stdBinFile file ("test.dat",BF_OPEN_RW);

  cout<<file.resize (1200)<<endl;;

  cout<<file.getpos ()<<endl;
}

