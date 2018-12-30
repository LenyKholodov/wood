#include <pch.h>
#include <memory\cache.h>
#include <debug.h>

///////////////////////implementation of class ClasterCache/////////////////////

void*   ClasterCache::ResAlloc    (size_t size,Resource& res)
{
  if (size > mClasterSize)
    return NULL;

  for (lump_t* lump=mLast.prev;lump!=&mFirst && lump->res && lump->res->IsLock ();lump=lump->prev);

  if (lump == &mFirst)
    return NULL;
  
  if (lump->res) 
    refresh (*lump->res);

  lump->res  = &res;
  buf (res) = lump;

  return lump->buf;
}

void*   ClasterCache::lock     (Resource& res)
{
  lump_t* lump     = (lump_t*)buf (res);

  lump->prev->next = lump->next;
  lump->next->prev = lump->prev;

  lump->prev       = &mFirst;      
  lump->next       = mFirst.next;
  mFirst.next      = mFirst.next->prev = lump;  

  return lump->buf;
}

void    ClasterCache::update (void* from,void* to)
{
  long offs = long (to) - long (from);

  for (lump_t* lump=mFirst.next;lump!=&mLast;lump=lump->next)
    lump->buf=(char*)lump->buf+offs;
}

void*   ClasterCache::load     ()
{
  char* data = (char*)alloc (mCacheSize);

  if (!data) 
    return NULL;

  int         prev = -1,
              next = +1;
  int         num  = mCacheSize / mClasterSize;

  for (int i=0;i<num;i++,prev++,next++,data+=mClasterSize)
  {
    lump_t& node = mNode [i];

    node.prev    = mNode + prev;
    node.next    = mNode + next;
    node.buf     = data;
    node.res     = NULL;
  }      

  mNode [num-1].next   = &mLast;
  mNode [0].prev       = &mFirst;
  mLast.prev           = mNode + num - 1;
  mFirst.next          = mNode;
  mLast.next           = mFirst.prev = NULL;
  mLast.buf            = mFirst.buf  = NULL;
  mLast.res            = mFirst.res  = NULL;

  return data;
}

void    ClasterCache::unload   ()
{
  int num = mCacheSize / mClasterSize;

  for (int i=0;i<num;i++)
    mNode [i].buf = NULL;
}

ClasterCache::ClasterCache    (Cache* owner,size_t cache_size,size_t claster_size)
      : Cache        (owner),
        mClasterSize (claster_size),
        mCacheSize   (cache_size),
        mNode        (NULL)
{
  mNode  = (lump_t*)GetPool()->alloc (mCacheSize / mClasterSize * sizeof (cachenode_t));

  if (!mNode)
    throw ExcNoMemory ("ClasterCache::ClasterCache",mCacheSize / mClasterSize * sizeof (cachenode_t),this);
}

ClasterCache::~ClasterCache   ()
{
  if (mNode) GetPool ()->free (mNode);
}

///////////////////////implementation of class DefragCache//////////////////////////////////

BOOL   DefragCache::MoveData  (void* from,void* to,DWORD par)
{
  if (!to)
    return FALSE;

  long       offs  = long (to) - long (from);
  Resource*  res   = (Resource*)par;
  Cache*     cache = res->owner ();

  if (!Cache::refresh (*res,(uchar*)buf (*res) + offs))
    return FALSE;

  buf (*res) = (uchar*)buf (*res) + offs;

  return TRUE;
}

void  DefragCache::start        (DefragCache::lump_t* node)
{
  if (node)
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->prev       = &mFirst;      
    node->next       = mFirst.next;
    mFirst.next      = mFirst.next->prev = node;      
  }  
}

void   DefragCache::end          (DefragCache::lump_t* node)
{
  if (node)
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next       = &mLast;
    node->prev       = mLast.prev;
    mLast.prev       = mLast.prev->next = node;
  }  
}

void*   DefragCache::ResAlloc   (size_t size,Resource& res)
{
  D_Try
  {
//    dout<<"dc alloc:\t"<<size<<" free "<<mResPool.memavail ()
//        <<" ("<<this<<" for "<<&res<<")"<<endl;

    size += sizeof (lump_t);

    if (size > mResPool.size ())
      return NULL;

    size_t free = mResPool.memavail ();

    if (free < size)
    {
      lump_t* prev = (lump_t*)mLast.prev->prev;

      for (lump_t* node = (lump_t*)mLast.prev;node!=&mFirst && free < size;node=prev,prev=(lump_t*)prev->prev)
        if (!node->res->IsLock ())
        {
          free += node->size;     
          refresh (*node->res);
        }

      if (mResPool.memavail() < size)  return NULL;
    }

    lump_t* node = (lump_t*)mResPool.alloc (size,MoveData,(DWORD)&res);

    if (!node)
    {
      mResPool.defrag (); //size ??
      node = (lump_t*)mResPool.alloc (size,MoveData,(DWORD)&res);

      if (!node)
        return NULL;
    }

    node->buf    = (char*)node + sizeof (lump_t);
    node->res    = &res;
    node->prev   = &mFirst;
    node->next   = mFirst.next;    
    node->size   = size;

    mFirst.next->prev = node;
    mFirst.next       = node;

    buf (res)        = node;

//    dout<<"dc memavail: "<<mResPool.memavail ()<<endl;

    return node->buf;
  }
  D_ExcRet ("DefragCache: Error at alloc",NULL);  
}

void    DefragCache::ResFree    (Resource& res)
{
  D_Try
  {
    lump_t* node = (lump_t*)buf (res);

//    dout<<"dc free "<<node->size<<" ("<<this<<" for "<<&res<<")"<<endl;

    if (node)
    {
      node->prev->next = node->next;
      node->next->prev = node->prev;      

      mResPool.free (node);
    }
  }
  D_Exc ("DefragCache: Error at free");
}

void*   DefragCache::lock    (Resource& res)
{
  start ((lump_t*)buf (res));

  return ((lump_t*)buf (res))->buf;
}

void    DefragCache::update (void* from,void* to)
{
  long offs = long (to) - long (from);

  mResPool.MovePtr (offs);

  ResRing::iterator iter   = mResRing;
  int               count  = mResRing.count ();

  for (int i=0;i<count;i++,iter++)
  {
    lump_t* lump    = (lump_t*)buf (*iter);
    buf (*iter)     = (uchar*)buf (*iter) + offs;
    
    lump->prev      = (lump_t*)((char*)lump->prev + offs);
    lump->next      = (lump_t*)((char*)lump->next + offs);
  }  

  mFirst.next       = (lump_t*)((char*)mFirst.next + offs);
  mLast.prev        = (lump_t*)((char*)mLast.prev + offs);
}

void*   DefragCache::load    ()
{
  void* buf = alloc (mResPool.size());

  mResPool.setbuf (buf,mResPool.size());
  mResPool.reset  ();    

  return buf;
}

void    DefragCache::unload  ()
{
  mFirst.prev = mLast.next = NULL;
  mFirst.res  = mLast.res  = NULL;
  mFirst.buf  = mLast.buf  = NULL;
  mFirst.size = mLast.size = NULL;
  mFirst.next = &mLast;
  mLast.prev  = &mFirst;  

  mResPool.setbuf (NULL,mResPool.size());  
}

DefragCache::DefragCache  (Cache* cache,size_t size)
            : mResPool  (),
              Cache     (cache)
{
  mResPool.setbuf (NULL,size);

  mFirst.prev = mLast.next = NULL;
  mFirst.res  = mLast.res  = NULL;
  mFirst.buf  = mLast.buf  = NULL;
  mFirst.size = mLast.size = NULL;
  mFirst.next = &mLast;
  mLast.prev  = &mFirst;    
}

DefragCache::~DefragCache ()
{    }

////////////////////////////////implementation of class FlushCache///////////////////////////

void  FlushCache::start        (FlushCache::lump_t* node)
{
  if (node)
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->prev       = &mFirst;      
    node->next       = mFirst.next;
    mFirst.next      = mFirst.next->prev = node;      
  }  
}

void   FlushCache::end          (FlushCache::lump_t* node)
{
  if (node)
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next       = &mLast;
    node->prev       = mLast.prev;
    mLast.prev       = mLast.prev->next = node;
  }  
}

void*   FlushCache::ResAlloc   (size_t size,Resource& res)
{
  D_Try
  {
    size += sizeof (lump_t);

    if (size > mResPool.size ())
      return NULL;

    if (mResPool.maxavail () < size)
    {
      lump_t* prev = (lump_t*)mLast.prev->prev;

      for (lump_t* node = (lump_t*)mLast.prev;node!=&mFirst && mResPool.maxavail () < size;node=prev,prev=(lump_t*)prev->prev)
        if (!node->res->IsLock ())
          refresh (*node->res);

      if (mResPool.maxavail () < size)
        return NULL;
    }

    lump_t* node = (lump_t*)mResPool.alloc (size);

    node->buf    = (char*)node + sizeof (lump_t);
    node->res    = &res;
    node->prev   = &mFirst;
    node->next   = mFirst.next;    
    node->size   = size;

    mFirst.next->prev = node;
    mFirst.next       = node;

    buf (res)         = node;

    return node->buf;
  }
  D_ExcRet ("FlushCache: Error at alloc",NULL);  
}

void    FlushCache::ResFree    (Resource& res)
{
  D_Try
  {
    lump_t* node = (lump_t*)buf (res);

    if (node)
    {
      node->prev->next = node->next;
      node->next->prev = node->prev;      

      mResPool.free (node);
    }
  }
  D_Exc ("FlushCache: Error at free");
}

void*   FlushCache::lock    (Resource& res)
{
  start ((lump_t*)buf (res));

  return ((lump_t*)buf (res))->buf;
}

void    FlushCache::update (void* from,void* to)
{
  long offs = long (to) - long (from);

  mResPool.MovePtr (offs);

  ResRing::iterator iter   = mResRing;
  int               count  = mResRing.count ();

  for (int i=0;i<count;i++,iter++)
  {
    lump_t* lump    = (lump_t*)buf (*iter);
    buf (*iter)     = (uchar*)buf (*iter) + offs;
    
    lump->prev      = (lump_t*)((char*)lump->prev + offs);
    lump->next      = (lump_t*)((char*)lump->next + offs);
  }  

  mFirst.next       = (lump_t*)((char*)mFirst.next + offs);
  mLast.prev        = (lump_t*)((char*)mLast.prev + offs);
}

void*   FlushCache::load    ()
{
  void* buf = alloc (mResPool.size());

  mResPool.setbuf (buf,mResPool.size());
  mResPool.reset  ();    

  return buf;
}

void    FlushCache::unload  ()
{
  mFirst.prev = mLast.next = NULL;
  mFirst.res  = mLast.res  = NULL;
  mFirst.buf  = mLast.buf  = NULL;
  mFirst.size = mLast.size = NULL;
  mFirst.next = &mLast;
  mLast.prev  = &mFirst;  
  
  mResPool.setbuf (NULL,mResPool.size());  
}

FlushCache::FlushCache  (Cache* cache,size_t size)
            : mResPool  (),
              Cache     (cache)
{
  mResPool.setbuf (NULL,size);

  mFirst.prev = mLast.next = NULL;
  mFirst.res  = mLast.res  = NULL;
  mFirst.buf  = mLast.buf  = NULL;
  mFirst.size = mLast.size = NULL;
  mFirst.next = &mLast;
  mLast.prev  = &mFirst;    
}

FlushCache::~FlushCache ()
{    }