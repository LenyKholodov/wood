#define DEBUG
#include <misc.h>

void     main ()
{
  for (int i=0;i<256;i++)
    dout<<i<<' '<<(char)i<<" "<<toUpper [i]<<endl;
}