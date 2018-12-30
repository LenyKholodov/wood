#include <pch.h>
#include <kernel\thread.h>
#include <debug.h>

DWORD   Thread::mCurrentThread    = 0xFFFFFFFF;       
int     Thread::mTotalThreadCount = 0;

Pool*   KernelObject::mKernelPool = NULL;

DWORD  WINAPI   Thread::RunThread (void* threadPtr)
{   
  Thread*  thread = (Thread*)threadPtr;
  error_t  code   = 1;

  TlsSetValue (mCurrentThread,thread);    

  if (!thread)
    return 1;

  D_Try    
  {
    code = thread->process ();

    delete thread;
       
    return code;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("Thread: Error at thread: "<<GetCurrentThreadId());

    D_Try 
    {
      delete thread;
      return 1;
    }
    D_ExcRet ("Thread: Error at delete thread (at exception)",1);
  } 
}

void   Thread::RegisterCurrent   (Service& srv)
{
  CSEntry entry (mCS);
  mServCur.insert (&srv);
}

void   Thread::UnregisterCurrent (Service& srv)
{
  CSEntry entry (mCS);
  mServCur.erase (&srv);
}

void   Thread::RegisterAutodel   (Service& srv)
{
  CSEntry entry (mCS);
  mServDel.insert(&srv);
}

void   Thread::UnregisterAutodel (Service& srv)
{
  CSEntry entry (mCS);
  mServDel.erase(&srv);
}

void   Thread::RegisterThread (Thread* thread)
{
  CSEntry entry (mCS);

  mChildren.insert (thread);
}         

void   Thread::UnregisterThread (Thread* thread)
{
  CSEntry entry (mCS);

  mChildren.erase (thread);
}         

void   Thread::pause  () 
{ 
  SuspendThread (mHandle);  
}

void            Thread::resume () 
{
  if (!ResumeThread (mHandle))
  {
    Sleep (50);
    ResumeThread (mHandle);
  }  
}

int   Thread::count (bool flag) const
{
  CSEntry entry ((CritSect&)mCS);

  int total = mChildren.count ();

  if (flag)
  {
    ThreadRing::iterator iter  = mChildren;
    int                  count = mChildren.count ();

    for (int i=0;i<count;i++,iter++)
      total += iter->count (flag);
  }

  return total;
}

bool  Thread::StartThread   ()
{
  CSEntry trans (mCS);

  if (!mHandle)
  {
         //create system thread           

    mHandle = ::CreateThread (NULL,0,RunThread,this,0,&mThreadId);

    if (!mHandle) return false;
    else 
    {
      mTotalThreadCount++;  
      return true; 
    }  
  }

  return false;
}

void    Thread::StopThread (error_t err)
{
  if (mPresent)
  {
    D_Try
    {
      static Service*  srv     [32];
      static Thread*   threads [32];

      D_Try
      {
        mCS.enter ();

        ThreadRing::iterator iter  = mChildren;--iter;
        int                  count = mChildren.count();

        mCS.leave ();

        while (count)
        {
          uint cnt = count >= 32 ? 32 : count;

          mCS.enter ();

          for (int i=0;i<cnt;threads [i++] = (Thread*)iter--);          

          mCS.leave ();

          for (i=0;i<cnt;i++)
          {
            D_Try
            {
              delete threads [i];
            }
            D_Exception (EXCEPTION_EXECUTE_HANDLER)
            {
            }          
          }
  
          count -= cnt;
        }
      }
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      {
      }

      D_Try
      {     
        if (mServCur.count ())
        {
          ServiceRing::iterator iter  = mServCur;--iter;
          int                   count = mServCur.count();

          while (count)
          {
            uint cnt = count >= 32 ? 32 : count;

            for (int i=0;i<cnt;srv [i++] = (Service*)iter--);

            for (i=0;i<cnt;i++)
            {
              D_Try
              {
                srv [i]->unbind (this,err);
              }
              D_Exception (EXCEPTION_EXECUTE_HANDLER)
              {           
              }
            }  
          }  
        }

        if (mServDel.count ())
        {
          ServiceRing::iterator iter  = mServDel;--iter;
          int                   count = mServDel.count();

          while (count)
          {
            uint cnt = count >= 32 ? 32 : count;

            for (int i=0;i<cnt;srv [i++] = (Service*)iter--);

            for (i=0;i<cnt;i++)
            {
              D_Try
              {
                delete srv [i];
              }
              D_Exception (EXCEPTION_EXECUTE_HANDLER)
              {           
              }
            }  
          }  
        }

      }
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      { 
      }  

      D_Try
      {
        if (mOwner) mOwner->UnregisterThread (this);
      }      
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      {           
      }

      if (mHandle)
         ::TerminateThread (mHandle,err);

      mHandle = NULL;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }        

    mPresent = false;
  }  
}

Thread::Thread  (bool create)
       : mHandle   (NULL),
         mThreadId (GetCurrentThreadId ()),
         mPresent  (false),
         mServCur  (GetKernelPool()),
         mServDel  (GetKernelPool()),
         mChildren (GetKernelPool()),
         mOwner    (K_GetCurrentThread ())

{
  CSEntry trans (mCS);

  if (mCurrentThread == 0xFFFFFFFF)
  {
    mCurrentThread = TlsAlloc ();

    TlsSetValue (mCurrentThread,this);
  }  

  mTotalThreadCount++;

  if (mOwner)
    mOwner->RegisterThread (this);

  if (create)      
    StartThread ();    
}

Thread::~Thread ()
{
  StopThread ();

  if (!--mTotalThreadCount)
  {
    TlsFree (mCurrentThread);
    mCurrentThread = 0xFFFFFFFF;
  }
}

Thread* K_GetCurrentThread  () 
{
  return (Thread*)TlsGetValue (Thread::mCurrentThread);
}
