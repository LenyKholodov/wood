#include <pch.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <log.h>

#include <default.h>

class    EmulThread: public Thread
{
  public:
        EmulThread (): Thread (false) {
          DuplicateHandle (GetCurrentProcess (),
                           GetCurrentThread (),
                           GetCurrentProcess (),
                           &mHandle,
                           0,
                           FALSE,
                           DUPLICATE_SAME_ACCESS
                          );
        }
};

#pragma comment (lib,"user32.lib")

class    DefaultInit
{
  public:       
        DefaultInit  (): mHeap (NULL), mThread (NULL) {
          mHeap   = new (&mSysPool) FloatPool (&mSysPool,mSysPool.memavail () * 0.5f);
//          mHeap   = new (mHeap) SharedPool (mHeap,true); //???

//          KernelObject::mKernelPool = new (mHeap) SharedPool (mHeap,false);
          KernelObject::mKernelPool = mHeap;

          mThread = new (mHeap) EmulThread ();   

          SrvMakeCurrent (M_Load (mHeap));
          SrvMakeCurrent (F_Load (mHeap));
        }
        ~DefaultInit () { 
          F_Done ();                     
          M_Done ();          
          
          if (mThread) delete mThread; 

          if (KernelObject::mKernelPool)
          {
            delete KernelObject::mKernelPool;
            KernelObject::mKernelPool = NULL;
          }
 
          if (mHeap)   delete mHeap;
        }

        void*   operator new     (size_t size) { return malloc (size); }
        void    operator delete  (void* x)     { free (x);             }

   SystemPool      mSysPool;
   Pool*           mHeap;  
   Thread*         mThread;
};

typedef  void* (*AllocFn)(size_t);
typedef  void  (*FreeFn)(void*);

static DefaultInit*   __defaultMan         = NULL;
static AllocFn        __allocFn            = StdAlloc;
static FreeFn         __freeFn             = StdFree;
static uint           initCount            = 0;
uint                  StaticInit::mCount   = 0;   

extern Log*           logOut               = NULL;
extern ostream*       debugstream          = &cerr;
extern Ticker*        kernelTicker         = NULL;

bool           K_Init (uint count,...)
{
  if (!initCount++)
  {
    va_list list;

    debugstream  = new ofstream ("runlog");

    __defaultMan = new DefaultInit;        
    __allocFn    = MemoryService::AllocChunk;
    __freeFn     = MemoryService::FreeChunk;

    logOut       = new Log (&dout); ///old
    kernelTicker = new Ticker (KernelObject::mKernelPool);

    if (!__defaultMan)
      return false;

    va_start (list,count);

    for (int i=0;i<count;i++)
    {
      ServiceInitFn fn = va_arg (list,ServiceInitFn);

      if (!(*fn)())
        return false;
    }

    va_end   (list);

    return true;
  }

  return false;
}

void            K_Done (uint count,...)
{
  if (!--initCount)
  {
    D_Try
    {
      va_list list;  

      va_start (list,count);

      for (int i=0;i<count;i++)
      {
        ServiceDoneFn fn = va_arg (list,ServiceDoneFn);

        (*fn)();
      }

      va_end   (list);

      if (kernelTicker) delete kernelTicker;

      kernelTicker = NULL;

      if (logOut)       delete logOut; ///old

      if (__defaultMan) delete __defaultMan;

      __allocFn    = StdAlloc;
      __freeFn     = StdFree;

      if (debugstream && debugstream != &cerr) delete debugstream;
      debugstream = &cerr;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      dout<<"Error at K_Done"<<endl;
    }

    __allocFn    = StdAlloc;
    __freeFn     = StdFree;
  }
}

//////////////////////Распределители памяти//////////////////////////////////////////////////

void*       operator  new     (size_t bytes,Pool* pool) throw ()
{
  return M_Alloc (bytes,pool); 
}

void*       operator new   []  (size_t bytes,Pool* pool) throw ()
{
  return M_Alloc (bytes,pool); 
}

void*    operator  new       (size_t size)  throw ()
{ 
  return (*__allocFn)(size);
}

void*    operator  new []    (size_t size)  throw ()
{ 
  return (*__allocFn)(size);
}  

void     operator  delete    (void* x)  
{ 
  (*__freeFn)(x);
}

void     operator  delete [] (void* x)  
{ 
  (*__freeFn)(x);
}  

void* StaticInit::operator new (size_t size)
{
  return malloc (size);
}

void  StaticInit::operator delete (void* x)
{
  free (x);
}