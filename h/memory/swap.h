///////////////////////////////////////////////////////////////////////////////
//Name:         cache.h
//System:       Memory-Cache
//Abstract:     Swap file
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_SWAP_FILE__
#define __MEMORY_SWAP_FILE__

#include <memory\pool.h>

class BinFile;

////////////////////////////////////////////////////////////////////////////////
///Swap-file
////////////////////////////////////////////////////////////////////////////////
struct   swalloc_t;
class    SwapFile: public MemObject
{
  public:
                        SwapFile  (Pool*,
                                   const char* fname,
                                   size_t _MaxSize     = 0,
                                   size_t _ClasterSize = 512);
          virtual       ~SwapFile ();

////////////////////////////////////////////////////////////////////////////////
///Информация
////////////////////////////////////////////////////////////////////////////////
          size_t        size      () const    { return mSize;  }
          size_t        maxavail  () const    { return mFree;  }
          size_t        memavail  () const    { return mFree;  }
          size_t        clsize    () const    { return mClaster;  }

/////////////////////////////////////////////////////////////////////////////////
///Запись и чтение из Swap-file
////////////////////////////////////////////////////////////////////////////////
          long          write     (const void*,size_t); //ret UID
          bool          read      (void* buf,long UID);
          
  protected:
          size_t        alloc     (swalloc_t*,size_t);
          void          free      (long fst);

          BOOL          check     (size_t);

  private:
          BinFile*    mFile;
          size_t      mSize, mClaster;
          long        mFree;
          long        mLastPos, mFirstPos;
          ulong       mShift, mMask;
};

#endif