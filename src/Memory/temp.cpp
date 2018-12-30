#include <pch.h>
#include <memory\temp.h>
#include <debug.h>

void*      TempPool::_alloc ( size_t bytes)
{
  if (bytes < mFree)
  {
    char*   current =  mPos;  

    mPos            += bytes;
    mFree           -= bytes;
    
    return current;
  }
  else return NULL;
}

void       TempPool::_reset  (void* pool)
{
  mPos  = (char*)pool;  
}

TempPool::TempPool  ( Pool* pool)
         : FlatPool (pool)
{
  mPos = (char*)getbuf();
}

TempPool::TempPool  ( Pool* pool,size_t size)
         : FlatPool (pool,size)
{
  mPos = (char*)getbuf();

//  if (mPos == NULL)
//    D_Error (EXCEPTION_NO_MEMORY);
}
