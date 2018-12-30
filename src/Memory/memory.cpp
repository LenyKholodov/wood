#include <pch.h>
#include <pool.h>
#include <debug.h>

#pragma pack (1)

struct memchunk_t
{
  Pool* pool;
  char  data [];
};

void* MemManager::operator new (size_t size,Pool* pool) throw (ExcBadAlloc&)
{
  D_Try
  {
    memchunk_t* obj = (memchunk_t*)pool->alloc (size + sizeof (memchunk_t));

    if (!obj)
      throw ExcBadAlloc ("MemManager::operator new",size,pool);

    obj->pool = pool;

    return obj->data;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("MemManager: Error at alloc bytes = "<<size<<" from pool: "<<pool);
    throw ExcBadAlloc ("MemManager::operator new",size,pool);    
  }
  
  return NULL;
}

void*       MemManager::operator new     (size_t,void* ptr)
{
  D_Try
  {
    memchunk_t* obj = (memchunk_t*)ptr;

    if (!obj)
      return NULL;
    
    obj->pool = NULL;

    return obj->data;
  }
  D_ExcRet ("MemManager: Error at alloc bytes using user defined new",NULL);
}

void        MemManager::operator  delete  (void* x)
{
  D_Try
  {
    memchunk_t* obj = (memchunk_t*)((char*)x - sizeof (memchunk_t));

    if (obj && obj->pool) 
      obj->pool->free   (obj);
  }  
  D_Exc ("MemManager: Error delete object");
}

void*   MemObject::alloc (size_t size)
{
  return IsValid (mPool) ? mPool->alloc (size) : NULL;
}

void    MemObject::free (void* x)
{
  if (IsValid (mPool))
    mPool->free (x);
}

MemObject::MemObject  (Pool* pool)
          : mPool (pool)
{     }

MemObject::MemObject  (): mPool (::M_GetPool ())  {  }

bool     IsValid (const MemObject* obj)
{
  D_Try
  {
    return obj ? obj->valid (): FALSE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    return false;
  }
}

bool     IsValid (const MemObject& obj)
{
  D_Try
  {
    return &obj ? obj.valid () : false;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    return false;
  }
}

