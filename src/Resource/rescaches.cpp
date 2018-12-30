#include <pch.h>
#include <resource\caches.h>
#include <debug.h>

////////////////////////////////DefragCache//////////////////////////////////////////////////

BOOL    DefragCache::MoveData      (void* from,void* to,DWORD)
{
  if (!to)
    return FALSE;

  cachebuffer_t* src   = (cachebuffer_t*)from;
  cachebuffer_t* dst   = (cachebuffer_t*)to;

  Resource&  res   = *src->res;

  if (res.IsLock ())
    return FALSE;

  if (!update (res,src->buf,dst->buf))
    return FALSE;

  DefragCache* cache = (DefragCache*)res.GetOwner ();
 
  cache->UpdateBuffer (src,dst);

  return TRUE;
}

void*   DefragCache::_alloc        (size_t size)
{
  if (size > this->size ())
    return NULL;

  size_t free = memavail ();

  if (free < size)
  {
    cachebuffer_t* prev = PrevBuffer (GetLastBuffer ());
    cachebuffer_t* end  = PrevBuffer (GetFirstBuffer());

    for (cachebuffer_t* node = GetLastBuffer ();node!= end && free < size;node=prev,prev=PrevBuffer (prev))
      if (!node->res->IsLock ())
      {
        free += node->size;
        this->free (node);
      }  

    if (memavail() < size)  return NULL;
  }

  void* buf = ((DefragPool*)mDataPool)->alloc (size,MoveData);

  if (!buf)
  {
    ((DefragPool*)mDataPool)->defrag ();
    buf = ((DefragPool*)mDataPool)->alloc (size,MoveData);

    if (!buf)
      return NULL;
  }

  return buf;
}

DefragCache::DefragCache  (Pool* pool,size_t size)
            : FlatCache<DefragPool> (pool,size)
{    }

/////////////////////////////ClasterCache////////////////////////////////////////////////////

void*   ClasterCache::_alloc (size_t size)
{
  if (size > ((ClasterPool*)mDataPool)->claster ())
    return NULL;

  void* buf = mDataPool->alloc (size);

  if (!buf)
  {
    cachebuffer_t* end  = PrevBuffer (GetFirstBuffer());

    for (cachebuffer_t* node = GetLastBuffer ();node!=end;node=PrevBuffer (node))
      if (!node->res->IsLock ())
      {
        this->free (node);
        break;
      }     
  }
  else return buf;

  return mDataPool->alloc (size);
}

ClasterCache::ClasterCache (Pool* pool,size_t size,size_t csize)
             : FlatCache<ClasterPool> (pool,true) 
{    
  mDataPool = new (pool) ClasterPool (pool,size,csize);
}

/////////////////////////////////////FlushCache//////////////////////////////////////////////

void*   FlushCache::_alloc        (size_t size)
{
  if (size > this->size ())
    return NULL;

  size_t free = memavail ();

  if (free < size)
  {
    cachebuffer_t* prev = PrevBuffer (GetLastBuffer ());
    cachebuffer_t* end  = PrevBuffer (GetFirstBuffer());

    for (cachebuffer_t* node = GetLastBuffer ();node!= end && free < size;node=prev,prev=PrevBuffer (prev))
      if (!node->res->IsLock ())
      {
        free += node->size;
        this->free (node);
      }  

    if (memavail() < size)  return NULL;
  }

  return mDataPool->alloc (size);
}

FlushCache::FlushCache  (Pool* pool,size_t size)
            : FlatCache<DefragPool> (pool,size)
{    }