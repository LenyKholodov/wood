#include <pch.h>
#include <memory\flcache.h>
#include <debug.h>
#include <string.h>

///////////////////////implementation of class FloatCache///////////////////////

void            FloatCache::lock       (dcachenode_t* node)
{
  D_Try
  {
    if (node)
    {
      start (node);

      node->lock++;
    }  
  } 
  D_Exc ("FloatCache: Error at lock");
}

void            FloatCache::unlock       (dcachenode_t* node)
{
  D_Try
  {
    if (node)
      node->lock--;
  } 
  D_Exc ("FloatCache: Error at unlock");
}

void            FloatCache::reset        ()
{
  D_Try
  {
    for (dcachenode_t* node = (dcachenode_t*)mFirst.next;node!=&mLast;node=(dcachenode_t*)node->next)
      node->lock = FALSE;
  }
  D_Exc ("FloatCache: Error at reset");
}

void            FloatCache::start        (dcachenode_t* node)
{
  D_Try
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
  D_Exc ("FloatCache: Error at start");
}

void            FloatCache::end          (dcachenode_t* node)
{
  D_Try
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
  D_Exc ("FloatCache: Error at end");
}


dcachenode_t*   FloatCache::AllocChunk   (size_t size)
{
  D_Try
  {
    size += sizeof (dcachenode_t);

    if (size > mDefragPool.size ())
      return NULL;

    uint free = mDefragPool.memavail ();

    if (free < size)
    {
      dcachenode_t* prev = (dcachenode_t*)mLast.prev->prev;

      for (dcachenode_t* node = (dcachenode_t*)mLast.prev;node!=&mFirst && free < size;node=prev,prev=(dcachenode_t*)prev->prev)
        if (!node->lock)
        {
          free            += node->size;
          FreeChunk (node);         
        }

      if (free < size)
      {  
        if (mFlags & FLUSH_EVENT)
          Flush ();

        dcachenode_t* prev = (dcachenode_t*)mLast.prev->prev;

        for (dcachenode_t* node = (dcachenode_t*)mLast.prev;node!=&mFirst && free < size;node=prev,prev=(dcachenode_t*)prev->prev)
          if (!node->lock)
          {
            free            += node->size;
            FreeChunk (node);         
          }          

        if (free < size)  return NULL;
      }   
    }

    cachenode_t* node = (cachenode_t*)mDefragPool.alloc (size,Move,(DWORD)this);

    if (!node && !(mFlags&NO_MOVE))
    {
      if (mFlags & FLUSH_EVENT)
        Flush ();

      mDefragPool.defrag         ();
      node = (cachenode_t*)mDefragPool.alloc   (size,Move,(DWORD)this);

      if (!node)
        return NULL;
    }

    node->buf         = (char*)node + sizeof (dcachenode_t);
    node->prev        = &mFirst;
    node->next        = mFirst.next;    
    node->index       = -1;

    mFirst.next->prev = node;
    mFirst.next       = node;

    dcachenode_t* extNode = (dcachenode_t*)node;

    extNode->lock     = FALSE;
    extNode->size     = size;

    return extNode;
  }
  D_ExcRet ("FloatCache: Error at AllocChunk",NULL);
}

void         FloatCache::FreeChunk    (dcachenode_t* node)
{
  D_Try
  {
    if (node)
    {
      node->prev->next = node->next;
      node->next->prev = node->prev;

      if (node->lock && (mFlags & FLUSH_EVENT))
        Flush ();
      if (!(mFlags & READ_ONLY)) UnloadData (node);

      mDefragPool.free (node);
    }
  }
  D_Exc ("FloatCache: Error at FreeChunk");
}

BOOL         FloatCache::Move         (void* from,void* to,DWORD par)
{
  FloatCache*     cache = (FloatCache*)par;
  dcachenode_t*   src   = (dcachenode_t*)from;
  dcachenode_t*   dest  = (dcachenode_t*)to;

  if (src->lock)
    return FALSE;

  if (cache->mFlags & MOVE_DATA_EVENT)
    if (!cache->MoveData (src->param,(char*)from+sizeof (dcachenode_t),(char*)to+sizeof (dcachenode_t)))
      return FALSE;

  src->next->prev       = dest;
  src->prev->next       = dest;
  src->buf              = (char*)src->buf + ulong (dest) - ulong (src);

  return TRUE;  
}

FloatCache::FloatCache (Pool* _Pool,size_t _CacheSize,uint _Flags)
            : MemObject   (_Pool),
              mDefragPool (_Pool,_CacheSize),
              mFlags      (_Flags)
{
//  D_Try
//  {
    if (!IsValid (&mDefragPool))
    {
      D_Message ("FloatCache: No enough memory for defrag pool");
      return;
    }

    mFirst.prev  = mLast.next = NULL;
    mFirst.next  = &mLast;
    mLast.prev   = &mFirst;
    mFirst.size  = mLast.size = 0;
    mFirst.lock  = mLast.lock = FALSE;
    mFirst.index = mLast.index = -1;
    mFirst.buf   = mLast.buf = NULL;
//  }
//  D_Exc ("FloatCache: Error at construct");
}

FloatCache::~FloatCache ()
{    }
