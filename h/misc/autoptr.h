#ifndef __MISC_AUTO_POINTER__
#define __MISC_AUTO_POINTER__

#include <types.h>

template <class T> class autoptr_t
{
  public:                        
                        autoptr_t  (T* obj = NULL): mObj (obj) {}
                        ~autoptr_t () { if (mObj) delete mObj; }

        T* operator = (const T* x) { return mObj = (T*)x; }

        T*  operator -> () const { return mObj; }                
        operator T*     () const { return mObj; }        

  private:
        T*      mObj;
};

#endif