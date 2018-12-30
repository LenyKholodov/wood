#include <malloc.h>
#include <iostream.h>

void     main ()
{
  void* x = alloca (64*1024);

  cout<<x<<endl;
}