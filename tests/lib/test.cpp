#define  DEBUG
#include <memory\memory.h>
#include <iostream.h>

const size_t TEST_POOL_SIZE = 16*1024*1024;

void     main ()
{
  SystemPool    sys;
  Pool*         pool = new (&sys) FloatPool (&sys,TEST_POOL_SIZE);

  void*         x    = pool->alloc (1024*16);
  void*         y    = pool->alloc (1024*16);

  pool->free (y);

  cout<<"Check for x-pointer belong: "<<pool->belong (x)<<endl; 
  cout<<"Check for y-pointer belong: "<<pool->belong (y)<<endl;

  delete pool;
}