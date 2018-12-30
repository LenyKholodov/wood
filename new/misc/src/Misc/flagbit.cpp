#include <pch.h>

char     FlagBit::mMask  [8] = {1,2,4,8,16,32,64,128};

void     FlagBit::reset    ()      { memset (mFlags,0,mFlagsSize);         }

FlagBit::FlagBit  (Pool* _Pool,int num)
        : MemObject (_Pool),
          mFlags    ((char*)_Pool->alloc ((num>>3) + 1)),
          mFlagsSize ((num>>3) + 1)
{  
  reset ();
}

FlagBit::~FlagBit ()
{
  if (mFlags) free (mFlags);
}