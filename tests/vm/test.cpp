#include <windows.h>
#include <iostream.h>

const int TOTAL = 1024;

void     main ()
{
  long buf [TOTAL] = {0};
  long num         = TOTAL;

  cout<<AllocateUserPhysicalPages (GetCurrentProcess(),&num,buf)<<endl;;

  cout<<num<<endl;
}