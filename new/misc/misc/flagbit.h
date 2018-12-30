#ifndef __FLAG_BIT_SYSTEM__
#define __FLAG_BIT_SYSTEM__

#include <types.h>

class    Pool;

////////////////////////////////////////////////////////////////////////////////
///Флаговый класс 1/0 без проверки переполнения границ
////////////////////////////////////////////////////////////////////////////////
class  FlagBit: public MemObject
{
  public:
                FlagBit  (Pool*,int num);
     virtual    ~FlagBit ();

     inline  void       set      (int i) { mFlags [i>>3] |= mMask [i&7];          }
     inline  void       clear    (int i) { mFlags [i>>3] &= (0xFF ^ mMask [i&7]); }
             void       reset    ();

     inline  BOOL       check    (int i) const { return mFlags [i>>3] & mMask [i&7];    }

     inline  const BOOL operator [] (int i) const { return check (i) != 0; }

     virtual bool       valid   () const { return mFlags != NULL; }
  
  protected:
    static char       mMask [8];
           char*      mFlags;
           int        mFlagsSize;
};

#endif