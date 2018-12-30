////////////////////////////////////////////////////////////////////////////////
///Name:        defs.h
///System:      Thread
///Abstract:    Defines for thread system
////////////////////////////////////////////////////////////////////////////////

#ifndef __KERNEL_DEFS__
#define __KERNEL_DEFS__

#include <memory\object.h>
#include <misc.h>        

typedef  int               error_t;       

class  Pool;
class  Service;
class  Thread;

typedef  Ring<Service*>    ServiceRing;
typedef  Ring<Thread*>     ThreadRing;

class  KernelObject: private MemObject
{
  friend class DefaultInit;
  friend bool  K_Init (uint,...);
  public:
               KernelObject ():MemObject (mKernelPool) {}       

        virtual bool valid () const { return mKernelPool != NULL; }

        void*   operator new    (size_t size,Pool* pool = mKernelPool) { return MemObject::operator new (size,pool); }
        void*   operator new [] (size_t size,Pool* pool = mKernelPool) { return MemObject::operator new (size,pool); }
                MemObject::operator delete;
  
  protected:
        Pool*  GetKernelPool () const { return mKernelPool; }

               MemObject::GetPool;
               MemObject::SetPool;
               MemObject::M_GetPool;

  private:
        static Pool*    mKernelPool;
};

#endif