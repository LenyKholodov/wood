#include <pch.h>
#include <console\strpool.h>

char*  StringPool::alloc   (size_t size,char** ptr)
{
  void* data = DefragPool::alloc (size,ptr);

  if (!data)
  {
    defrag ();
    data = DefragPool::alloc (size,ptr);    
  }

  return (char*)data;
}

char*  StringPool::realloc (size_t size,char** ptr)
{
  D_Try
  {
    if (!ptr)
      return NULL;

    char* x   = *ptr;
    char* old = *ptr;
    x         = StringPool::alloc (size,ptr);

    if (!x) 
    {
      *ptr = x;
      return NULL;
    }        

    DefragPool::free (old);

    return *ptr = x;
  }
  D_ExcRet ("StringPool: Error at realloc",NULL);
}

StringPool::StringPool  (Pool* _Pool,size_t _Size)
           : DefragPool (_Pool,_Size)                          
{  }