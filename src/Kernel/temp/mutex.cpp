#include <pch.h>
#include <kernel\mutex.h>

void Mutex::lock () const
{
  WaitForSingleObject (mHandle,INFINITE);  
}

void Mutex::unlock () const
{
  ReleaseMutex (mHandle);
}

Mutex::Mutex  ()
{
  mHandle = CreateMutex (NULL,FALSE,NULL); 
}

Mutex::~Mutex ()
{
  CloseHandle (mHandle);
}
