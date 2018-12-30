#ifndef __MEMORY_HEAP_POOL__
#define __MEMORY_HEAP_POOL__

#include <memory\float.h>
#include <misc\dict.h>

const size_t DEFAULT_HEAP_START_SIZE  = 4096 * 16;
const int    DEFAULT_HEAP_TEMP_SIZE   = 4096;

////////////////////////////////////////////////////////////////////////////////
///Куча
////////////////////////////////////////////////////////////////////////////////
class  HeapPool: public Pool
{
  public:
                HeapPool (Pool*,
                          size_t _StartSize = DEFAULT_HEAP_START_SIZE
                         );
   virtual      ~HeapPool ();

          virtual       size_t  size     () const;
          virtual       size_t  memavail () const;
          virtual       size_t  maxavail () const;

          virtual       void    reset   ( void   );
          virtual       bool    belong  ( void*  ) const;

  protected:
          virtual       void*   _alloc   ( size_t );
          virtual       void    _free    ( void*  ); 

          struct  lump_t
          {
            size_t      size;
            char        data [];
          };

          typedef Dict<lump_t*> LTree;

                        void    clear    (LTree::iterator);
                        void    clear    ();
                        void*   find     (LTree::iterator,size_t);

  private:          
          FloatPool     mStartPool;
          FloatPool     mTempPool;
          LTree         mLumpTree;
          size_t        mFree, mMaxFree;          
};  

#endif