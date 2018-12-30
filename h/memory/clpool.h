///////////////////////////////////////////////////////////////////////////////
//Name:         clpool.h
//System:       Memory
//Abstract:     Claster pool for fixed memory mapped
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_CLASTER_POOL__
#define __MEMORY_CLASTER_POOL__

#include <memory/flat.h>
#include <debug\defs.h>

struct  claster_t;

///////////////////////////////////////////////////////////////////////////////
///Пул разбитый на кластеры одинакового размера
///////////////////////////////////////////////////////////////////////////////
class   ClasterPool: public FlatPool
{
  public:
                      ClasterPool ( Pool*,size_t cl_size);
                      ClasterPool ( Pool*,size_t total_size,size_t cl_size);

   virtual bool       belong   (void*) const;

   virtual size_t     maxavail () const;
   virtual size_t     memavail () const;
           size_t     clsize   () const;
           int        clcount  () const;

           size_t     claster  () const { return clsize();  }
           int        num      () const { return clcount(); }

  protected:
   virtual void*      _alloc  ( size_t );
   virtual void       _free   ( void*  );

  private:
   virtual void       _reset   (void*);

  private:
           size_t       mClaster;
           int          mClasterNum;
           claster_t*   mFirst;
};

////////////////////////////////////////////////////////////////////////////////
///Шаблон для списочного выделения
////////////////////////////////////////////////////////////////////////////////
template <class Type> class  ClPool: public MemObject
{
  public:
                ClPool  (Pool* pool,int num);
       virtual  ~ClPool ();

       virtual bool valid () const { return mTable != NULL; }

       inline Type*    alloc   ();
       inline void     free    (Type*);
       inline int      index   (Type* obj) { return ((ulong)obj-sizeof (node_t*)-ulong(mTable)) / sizeof (node_t); }
       inline Type*    data    (int i) const { return (Type*)&mTable [i].obj; }

       inline int      num     () const { return mNum; }

       inline Type*    operator []  (int i) { return &mTable [i].obj; }

  private:            
       struct node_t
       {
         node_t*   next;
         Type      obj;
       };

       node_t*    mTable;
       node_t*    mFirst;
       int        mNum;       
};

template <class Type> inline  Type*  ClPool<Type>::alloc   ()
{
  node_t* _free = mFirst;

  if (!_free)
    return NULL;

  mFirst = mFirst->next;

  return &_free->obj;
}

template <class Type> inline void  ClPool<Type>::free  (Type* obj)
{
  if (obj)
  {
    node_t* node = (node_t*)((char*)obj-sizeof (node_t*));
    
    node->next   = mFirst;
    mFirst       = node;
  }  
}

template <class Type>
ClPool<Type>::ClPool  (Pool* pool,int num)
       : MemObject (pool),
         mFirst    (NULL),
         mTable    (NULL),
         mNum      (num)
{
  mTable = (node_t*)pool->alloc (sizeof (node_t) * num);
  mFirst = mTable;

  if (!mTable)
  {
    #ifdef DEBUG
      dout<<"ClPool: No enough memory "<<endl;
    #endif
    mNum = 0;
    return;
  }
  
  for (int i=0;i<num;i++)
    mTable [i].next = &mTable [i+1];

  mTable [i-1].next = NULL;
}

template <class Type>
ClPool<Type>::~ClPool ()
{
  if (mTable) GetPool()->free (mTable);
}

#endif