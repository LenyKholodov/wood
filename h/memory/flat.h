///////////////////////////////////////////////////////////////////////////////
//Name:         flat.h
//System:       Memory
//Abstract:     Abstract class for all flat pools
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_FLAT_POOL__
#define __MEMORY_FLAT_POOL__

#include <memory/pool.h>

///////////////////////////////////////////////////////////////////////////////
///Пул с привязанным адресным куском
//////////////////////////////////////////////////////////////////////////////
class   FlatPool: public Pool
{
  protected:
                                FlatPool  ();
  public:
                                FlatPool  (Pool*);
                                FlatPool  (Pool*,size_t);                                       
          virtual               ~FlatPool ();

          virtual       void    reset   ( void  );
          virtual       bool    belong  ( void* ) const;

          virtual       size_t  size     () const { return mSize; }
          virtual       size_t  memavail () const { return mFree; }
          
  protected:
          virtual       void    _reset   ( void*  )             = 0;
          
          void*                 getbuf   () const  { return mPoolSpace; }
          void                  setbuf   (void* x,size_t size = 0);
          
  private:
          void*         mPoolSpace;
          size_t        mSize;         
          
  protected:        
          size_t        mFree;                        
};

#endif