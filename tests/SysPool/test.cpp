#define  DEBUG
#include <memory\memory.h>
#include <debug.h>

void     main ()
{
  SystemPool    sys;
  
  void* buf = sys.alloc (1024*16);          

  dout<<buf<<endl;
  dout<<sys.belong (buf)<<endl;
  dout<<sys.belong (NULL)<<endl;
}