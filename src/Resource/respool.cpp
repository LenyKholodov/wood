#include <pch.h>
#include <memory\respool.h>

BOOL    ResourcePool::MoveData  (void* from,void* to,DWORD par)
{
  long       offs = long (to) - long (from);
  Resource*  res  = (Resource*)par;
  
  if (!Cache::refresh (*res,(uchar*)buf (*res) + offs))
    return FALSE;

  buf (*res) = (uchar*)buf (*res) + offs;

  return TRUE;
}

void*   ResourcePool::ResAlloc   (size_t size,Resource& res)
{ 
  void* buf = DefragPool::alloc (size,MoveData,(DWORD)&res);

  if (!buf)
  {
    DefragPool::defrag ();
    buf = DefragPool::alloc (size,MoveData,(DWORD)&res);
  }

  return Cache::buf (res) = buf;
}

void    ResourcePool::ResFree    (Resource& res)
{
  DefragPool::free  (buf (res));
}

void*   ResourcePool::lock    (Resource& res)
{
  return buf (res);
}

void    ResourcePool::update  (void* from,void* to)
{
  long offs = long (to) - long (from);

  MovePtr (offs);  

  ResRing::iterator iter   = mResRing;
  int               count  = mResRing.count ();

  for (int i=0;i<count;i++,iter++)
    buf (*iter) = (uchar*)buf (*iter) + offs;
}

void    ResourcePool::unload    ()
{
  DefragPool::setbuf (NULL);  
}

void*   ResourcePool::load      ()
{
  void* buf = alloc (size());

  DefragPool::setbuf (buf,size());
  DefragPool::reset  ();    

  return buf;
}

ResourcePool::ResourcePool  (Cache* cache,size_t size)
             : DefragPool (),
               Cache      (cache)
{   
  setbuf (NULL,size);
} 

ResourcePool::ResourcePool  (Pool* pool,size_t size)
             : DefragPool (pool,size),
               Cache      (pool)
{    }

ResourcePool::~ResourcePool ()
{   }
