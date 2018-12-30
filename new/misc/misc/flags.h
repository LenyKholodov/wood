#ifndef __MISC_FLAGS__
#define __MISC_FLAGS__

#include <types.h>

template <typename IntType> class Flags
{
  public:
                Flags (): mFlags (0) {}

/////////////////////////////////////////////////////////////////////////////////////////////
///Установка - снятие
/////////////////////////////////////////////////////////////////////////////////////////////
    inline      IntType   enable     (IntType x)       { return (mFlags |= x); }
    inline      IntType   disable    (IntType x)       { return (mFlags &= ~0 ^ x); }

    inline      IntType   set        (IntType x)       { return enable  (x); }
    inline      IntType   clear      (IntType x)       { return disable (x); }

    inline      IntType   flags      () const          { return mFlags; }
    
    inline      bool      check      (IntType x) const { return mFlags & x; }

  protected:
        IntType mFlags;
};                                   

typedef Flags<unsigned char>    Flags8;
typedef Flags<unsigned short>   Flags16;
typedef Flags<unsigned int>     Flags32;
typedef Flags<unsigned __int64> Flags64;

#endif

