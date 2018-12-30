#include <pch.h>
#include <misc.h>
#include <resource\cache.h>

typedef  Ring<cachebuffer_t*> ResDataRing;

#define ownerPool ((Cache*)mResource.GetOwner ())

//Приватные данные кэш-ресурса
class    CacheResourcePrivateData: public Pool
{
  friend class Cache;
  public:
                                CacheResourcePrivateData  (Resource& r):mResource (r),mFirst (NULL) {}
          virtual               ~CacheResourcePrivateData () {
            if (mFirst)
            {
              cachebuffer_t* next = mFirst->next;

              for (cachebuffer_t* data=mFirst;data;data=next,next=next?next->next:NULL)
                free (data->buf);
            }
          }

//Дублирование интерфейса пулов
          virtual       void    reset   () { ownerPool->reset (); }
          virtual       bool    belong  (void* x) const {              
            return ownerPool->belong ((char*)x-sizeof (cachebuffer_t)); 
          }

          virtual       size_t  size     () const { return ownerPool->size ();     }
          virtual       size_t  memavail () const { return ownerPool->memavail (); }
          virtual       size_t  maxavail () const { return ownerPool->maxavail (); }

          virtual       void    dump    (ostream& os) const { ownerPool->dump (os); }

          virtual       bool    valid    () const { return ownerPool->valid (); }

  protected:
          virtual       void*   _alloc   (size_t size) { 
            cachebuffer_t* data  = (cachebuffer_t*)ownerPool->_alloc (size + sizeof (cachebuffer_t)); 
            Cache*     cache = (Cache*)mResource.GetOwner ();
            
            if (!data)
              return NULL;

            data->res  = &mResource; 
            data->prev = NULL;
            data->next = mFirst;
            data->size = size;
            data->bufnode.buf = data;
            data->bufnode.prev = data->bufnode.next = NULL;

            cache->InsertBuffer (data);
            
            if (mFirst) mFirst = mFirst->prev = data;
            else        mFirst = data;           

            return data->buf;
          }

          virtual       void    _free    (void* x) { 
            cachebuffer_t* data  = (cachebuffer_t*)((char*)x-sizeof (cachebuffer_t));
            Cache*     cache = (Cache*)mResource.GetOwner ();
            
            if (data->prev) data->prev->next = data->next;
            else            mFirst           = data->next;
            if (data->next) data->next->prev = data->prev;

            Cache::update (mResource,data->buf,NULL); 

            cache->EraseBuffer (data);

            ownerPool->_free (data); 
          }

  private:
          Resource&     mResource;
          cachebuffer_t*    mFirst;
};

void   Cache::free   (cachebuffer_t* data)
{
  if (data && data->res && data->res->GetOwner () == this)
  {
    data->res->enable  (RES_DIRTY);
    data->res->disable (RES_LOAD);

    GetPool (*data->res)->free (data->buf);
  }
}

void   Cache::insert (Resource& r)
{
  data (r) = ext_new CacheResourcePrivateData (r);

  if (!data (r))
    return;

  ResourceGroup::insert (r);
}

bool   Cache::erase  (Resource& r)
{
  if (r.GetOwner () == this)
  {
    if (data (r))
      delete (CacheResourcePrivateData*)data (r);

    data (r) = NULL;

    return ResourceGroup::erase (r);
  }
  else return false;
}

bool   Cache::first (Resource& r)
{
  if (ResourceGroup::first (r))
  {
    CacheResourcePrivateData* data = (CacheResourcePrivateData*)ResourceGroup::data (r);

    if (!data)
      return false;

    for (cachebuffer_t* res=data->mFirst;res;res=res->next)
      SetFirstBuffer (res);

    return true;
  }
  else return false;
}

bool   Cache::last (Resource& r)
{
  if (ResourceGroup::first (r))
  {
    CacheResourcePrivateData* data = (CacheResourcePrivateData*)ResourceGroup::data (r);

    if (!data)
      return false;

    for (cachebuffer_t* res=data->mFirst;res;res=res->next)
      SetLastBuffer (res);

    return true;
  }
  else return false;
}

bool   Cache::lock   (Resource& r)
{
  if (ResourceGroup::lock (r))
  {
    if (r.IsDirty ())
    {
      if (!r.IsUpload ())  //если ресурс не умеет догружаться
        unload (r);

      r.disable (RES_LOAD); 
    }

    return true;
  }  
  else return false;
}

Pool*  Cache::GetPool (const Resource& r) const
{
  return (Pool*)data (r);
}

void  Cache::UpdateBuffer  (cachebuffer_t* src,cachebuffer_t* dst)
{
  if (src)
  {
    if (src->prev) src->prev->next = dst;
    else           
    {
      CacheResourcePrivateData* data = (CacheResourcePrivateData*)Cache::data (*src->res);
      data->mFirst                   = dst;
    }  
    if (src->next) src->next->prev = dst;

    if (src->bufnode.prev) src->bufnode.prev->next = &dst->bufnode;
    if (src->bufnode.next) src->bufnode.next->prev = &dst->bufnode; 
  }
}

void  Cache::InsertBuffer   (cachebuffer_t* data)
{
  if (data)
  {
    data->bufnode.next   = mBufFirst.next;
    data->bufnode.prev   = &mBufFirst;

    mBufFirst.next->prev = &data->bufnode;
    mBufFirst.next       = &data->bufnode;    
  }
}

void  Cache::EraseBuffer    (cachebuffer_t* data)
{
  if (data)
  {
    data->bufnode.prev->next = data->bufnode.next;
    data->bufnode.next->prev = data->bufnode.prev;    
  }
}

void  Cache::SetFirstBuffer (cachebuffer_t* data)
{
  if (data)
  {        
    data->bufnode.next->prev = data->bufnode.prev;
    data->bufnode.prev->next = data->bufnode.next;

    data->bufnode.next   = mBufFirst.next;
    data->bufnode.prev   = &mBufFirst;

    mBufFirst.next->prev = &data->bufnode;
    mBufFirst.next       = &data->bufnode;
  }
}

void  Cache::SetLastBuffer  (cachebuffer_t* data)
{
  if (data)
  {
    data->bufnode.next->prev = data->bufnode.prev;
    data->bufnode.prev->next = data->bufnode.next;

    data->bufnode.prev   = mBufLast.prev;
    data->bufnode.next   = &mBufLast; 

    mBufLast.prev->next  = &data->bufnode;
    mBufLast.prev        = &data->bufnode;
  }  
}

cachebuffer_t*   Cache::NextBuffer    (cachebuffer_t* data)
{
  if (data && data->bufnode.next) return data->bufnode.next->buf;
  else                            return NULL;  
}

cachebuffer_t*   Cache::PrevBuffer    (cachebuffer_t* data)
{
  if (data && data->bufnode.prev) return data->bufnode.prev->buf;
  else                            return NULL;
}

bool  Cache::EnumBuffers    (const Resource& r,Ring<cachebuffer_t*>& ring) const
{
  if ((Cache*)r.GetOwner () == this)
  {
    CacheResourcePrivateData* data = (CacheResourcePrivateData*)ResourceGroup::data (r);

    if (!data)
      return false;

    for (cachebuffer_t* res=data->mFirst;res;res=res->next)
      ring.insert (res);

    return true;  
  }

  return false;
}

Cache::Cache  (Pool* pool)
      : Pool (pool)
{  
  mBufFirst.buf  = mBufLast.buf = NULL;
  mBufFirst.next = &mBufLast;
  mBufLast.prev  = &mBufFirst;
  mBufFirst.prev = mBufLast.next = NULL;
}

Cache::~Cache ()
{
  UnlinkAll ();
}
