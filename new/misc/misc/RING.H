#ifndef __KERNEL_RING__
#define __KERNEL_RING__

#include <misc\list.h>

//////////////////////////////////////////////////////////////////////////////////////////////
///Кольцо 
//////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class RingTmpl: public List<T>
{
  public:
      RingTmpl  (Pool* pool = Memory::M_GetPool()): List<T>(pool) { reset (); }

      virtual void reset () { 
        List<T>::reset ();
        mFirst.prev = &mLast;
        mLast.next  = &mFirst;
      }
};

#define Ring RingTmpl

#endif