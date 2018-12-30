#ifndef __CACHE_UTILITES__
#define __CACHE_UTILITES__

#include <resource\cache.h>
#include <memory\dpool.h>
#include <memory\clpool.h>

template <class PoolType> class FlatCache: public Cache
{
  public:
                        FlatCache (Pool*,bool own);
                        FlatCache (Pool* pool,size_t size);
                        FlatCache (Pool*);
          virtual       ~FlatCache () ;

          virtual       void    reset   () { mDataPool->reset (); }
          virtual       bool    belong  (void* x) const { return mDataPool->belong (x); }

          virtual       size_t  size     () const { return mDataPool->size (); }
          virtual       size_t  memavail () const { return mDataPool->memavail(); }
          virtual       size_t  maxavail () const { return mDataPool->maxavail (); }

  protected:
          virtual void*   _alloc        (size_t size)  { return mDataPool->alloc (size); }
          virtual void    _free         (void* x)      { mDataPool->free (x);  }

  protected:
          Pool*  mDataPool;
          bool   mCreatePool;
};


////////////////////////////////////////////////////////////////////////////////
///Кэш с дефрагментацией
////////////////////////////////////////////////////////////////////////////////
class   DefragCache: public FlatCache<DefragPool>
{
  public:
                        DefragCache  (Pool*,size_t);        

  protected:
          virtual void*   _alloc        (size_t);

  private:
          static  BOOL    MoveData      (void* from,void* to,DWORD);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Кластерный кэш
/////////////////////////////////////////////////////////////////////////////////////////////
class ClasterCache: public FlatCache<ClasterPool>
{
  public:
                ClasterCache (Pool*,size_t pool_size,size_t claster_size);

  protected:
          virtual void*   _alloc        (size_t);
};

////////////////////////////////////////////////////////////////////////////////
///Кэш без дефрагментации 
////////////////////////////////////////////////////////////////////////////////
class   FlushCache: public FlatCache<DefragPool>
{
  public:
                        FlushCache      (Pool*,size_t);        

  protected:
          virtual void*   _alloc        (size_t);
};

/////////////////////////////////////////////////////////////////////////////////////////////
template <class PoolType>
FlatCache<PoolType>::FlatCache (Pool* pool,size_t size)
                    : Cache     (pool), 
                      mDataPool (new (pool) PoolType (pool,size)), 
                      mCreatePool (true) 
{  }

template <class PoolType>
FlatCache<PoolType>::FlatCache (Pool* pool)
                    : Cache     (pool),
                      mDataPool (pool),
                      mCreatePool (false)
{   }

template <class PoolType>
FlatCache<PoolType>::FlatCache (Pool* pool,bool own)
                    : Cache     (pool),
                      mDataPool (NULL),
                      mCreatePool (own)
{   }

template <class PoolType>
FlatCache<PoolType>::~FlatCache () 
{ 
  UnlinkAll ();

  if (mCreatePool && mDataPool)
    delete mDataPool;  
}


#endif 