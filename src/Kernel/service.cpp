#include <pch.h>
#include <kernel\thread.h>

bool       Service::bind        (Thread* thread)
{
  if (mThreadsUse.count () && !check (SRV_MULTITHREAD))
    return false;

  thread->RegisterCurrent (*this);
  mThreadsUse.insert      (thread);

  if (mCurContext)
   *mCurContext = this;

  return true;
}

void       Service::unbind      (Thread* thread,error_t err)
{
  thread->UnregisterCurrent (*this);
  mThreadsUse.erase         (thread);

  if (mCurContext)
   *mCurContext = NULL;
}

int        Service::GetError ()
{
  int err = mLastError; 
  mLastError = 0;

  return  err;
}

void       Service::SetError (int error)
{
  mLastError = error;
}

void        Service::BindDebugStream   (ostream& os)
{
  UnbindDebugStream ();

  dbg = os;
}

void        Service::BindDebugStream   (const char* file_name)
{
  UnbindDebugStream ();

  dbg = ext_new ofstream (file_name);

  set (SRV_OWNER_DEBUG_STREAM);
}

void        Service::UnbindDebugStream ()
{
  if (check (SRV_OWNER_DEBUG_STREAM))
    delete &dbg;

  dbg = NULL;      

  clear (SRV_OWNER_DEBUG_STREAM);
}

tickevent_t  Service::AddTimeEvent       (clock_t offs,tickdata_t event)
{
  return K_SetTimeEvent (*this,offs,event);
}

tickevent_t  Service::AddPeriodTimeEvent (clock_t offs,tickdata_t event)
{
  return K_SetPeriodTimeEvent (*this,offs,event);
}

void         Service::DelEvent           (tickevent_t event)
{
  K_DelTimeEvent (event);
}

Service::Service  (CurrentContextPtr* ptr)
        : mThreadsUse (GetPool()), 
          mLastError  (0), 
          dbg         (dout),
          mCurContext (ptr)
{    }

Service::~Service ()
{
  if (mThreadsUse.count ())
  {
    Thread* threads [32];
    int                  count = mThreadsUse.count ();
    ThreadRing::iterator iter  = mThreadsUse;iter--;

    while (count)
    {
      uint cnt = count >= 32 ? 32 : count;

      for (int i=0;i<cnt;threads [i++] = (Thread*)iter--);
      for (    i=0;i<cnt;unbind (threads [i++]));

      count -= cnt;
    }
  }

  if (check (SRV_OWNER_DEBUG_STREAM))     
    delete &dbg;
}

extern bool SrvMakeCurrent (Service* srv)
{
  D_Try
  {
    if (!srv)
      return false;

    if (!srv->mCurContext)
      return false;

    if (*srv->mCurContext)
      (*(threadptr_t<Service>*)srv->mCurContext)->unbind (K_GetCurrentThread ());

    return srv->bind (K_GetCurrentThread ());  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    return false;
  }
  
}