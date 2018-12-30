////////////////////////////////////////////////////////////////////////////////
//Name:         float.h
//System:       Memory
//Abstract:     Pool with float alloc
////////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_FLOAT_POOL__
#define __MEMORY_FLOAT_POOL__

#include <memory/flat.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
////Служебные
////////////////////////////////////////////////////////////////////////////////

const ulong  FREE_FLAG = 0xF1BA0A21;

#pragma pack(push,1)

struct allocdesc_t
{
  size_t        size;
  allocdesc_t*  prev;  
};

struct freedesc_t: public allocdesc_t
{
  ulong         id;
  freedesc_t*   prevFree;
  freedesc_t*   nextFree;
};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
///Пул с возможностью выделения блоков произвольного размера
////////////////////////////////////////////////////////////////////////////////
class   FloatPool: public FlatPool
{
  friend class ResourcePool;
  friend class DefragCache;
  friend class FlushCache;
  protected:
                                FloatPool  ();
  public:
                                FloatPool  (Pool*);
                                FloatPool  (Pool*,size_t);                                
          virtual               ~FloatPool ();

          virtual       bool    belong  ( void* ) const;
          
          virtual       size_t  maxavail () const;
          virtual       size_t  memavail () const; 

  protected:
          virtual       void*         _alloc   ( size_t );
          virtual       void          _free    ( void*  );          

          inline        freedesc_t*   next     ( const freedesc_t* node) const;

  protected:
                        freedesc_t*   _concat  ( freedesc_t* left,freedesc_t* right);
          virtual       void          _reset   ( void*  );
                        void          MovePtr  ( long offs ); //for the caches          
          
  protected:
          freedesc_t*   mFirstFree;
};

inline freedesc_t*  FloatPool::next (const freedesc_t* node) const
{
  if (!node)
    return NULL;
  else if ((char*)node - (char*)getbuf() + node->size < size ())  
     return (freedesc_t*)((char*)node+node->size);
  else            
     return NULL;
}


#endif