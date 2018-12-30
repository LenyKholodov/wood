#undef  _WIN32_WINNT
#define _WIN32_WINNT  0x0400

#include <pch.h>
#include <kernel\critsect.h>

void   CriticalSection::enter    ()
{
  EnterCriticalSection (&mSection);  
}

void   CriticalSection::leave    ()
{
  LeaveCriticalSection (&mSection);
}

bool   CriticalSection::tryenter ()
{
  return TryEnterCriticalSection (&mSection);
}

CriticalSection::CriticalSection   ()
{
  InitializeCriticalSection (&mSection);
}

CriticalSection::~CriticalSection  ()
{
  DeleteCriticalSection (&mSection);
}
