#include <pch.h>
#include <misc\threadptr.h>

CurrentContextPtr::CurrentContextPtr  ()
{
  mCurContext = NO_LINK;
}

CurrentContextPtr::CurrentContextPtr  (void* data) 
                  : mCurContext (NO_LINK)
{ 
  set (data); 
}

CurrentContextPtr::CurrentContextPtr  (const CurrentContextPtr& ptr)
{
  *this = ptr;
}

CurrentContextPtr::~CurrentContextPtr () 
{ 
  unlink (); 
}

CurrentContextPtr& CurrentContextPtr::set (void* data) 
{
  if (mCurContext == NO_LINK)
  {
    mCurContext = TlsAlloc ();            
    if (mCurContext == NO_LINK)
      return *this;
  }  

  TlsSetValue (mCurContext,data);

  return *this;
}

void CurrentContextPtr::unlink () 
{
  if (mCurContext != NO_LINK)
    TlsFree (mCurContext);
  mCurContext  = NO_LINK;
}