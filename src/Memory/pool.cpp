#include <pch.h>
#include <memory\pool.h>
#include <debug.h>

void*   Pool::alloc   (size_t bytes)
{
  D_Try //debug
  {
    if  (!bytes) bytes++;
    return _alloc (bytes);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("Pool: Error alloc block size = "<<bytes<<" from pool "<<this);
  }
  return NULL;
}

void    Pool::free    (void*  block)
{
  D_Try
  {
    if (belong (block))
          _free (block);
  }
  D_Exc ("Pool: Error at free block "<<block<<" from pool "<<this);
}

void    Pool::dump      (ostream& os) const
{
  os<<"Pool:\t"<<"(size = "<<size()<<")\t"<<"(free = "<<memavail()<<")\t"
    <<"(maxfree = "<<maxavail()<<")\t"<<endl;
}
                        
Pool::Pool  (Pool* own)
     : MemObject (own)
{  }

Pool::~Pool () 
{  
}                        
