#include <pch.h>
#include <kernel\sequence.h>

/////////////////////////////////SequenceObject/////////////////////////////////////////////

bool SequenceObject::open (uint count,SequenceObject* objects)
{
  HANDLE handles [32];

  if (count <= 32)
  {
    for (int i=0;i<count;handles [i++] = objects++->mHandle);

    return WaitForMultipleObjects (count,handles,TRUE,INFINITE) != WAIT_FAILED;
  }    
  else return false;
}

bool SequenceObject::open ()
{
  return WaitForSingleObject (mHandle,INFINITE) != WAIT_FAILED;
}

SequenceObject::SequenceObject (HANDLE hnd)
  : mHandle (hnd)
{  }

SequenceObject::~SequenceObject ()
{
  if (mHandle)
    CloseHandle (mHandle);
}

////////////////////////////Mutex///////////////////////////////////////////////////////////

Mutex::Mutex ()
  : SequenceObject (CreateMutex (NULL,FALSE,NULL))
{  }

void Mutex::release ()
{
  ReleaseMutex (mHandle);
}

////////////////////////Semaphore///////////////////////////////////////////////////////////

Semaphore::Semaphore  (uint max_count,uint start_count)
 : SequenceObject (CreateSemaphore (NULL,start_count?start_count:max_count,max_count,NULL))
{ }
               
void Semaphore::release ()
{
  ReleaseSemaphore (mHandle,1,NULL);
}

//////////////////////////////Event/////////////////////////////////////////////////////////

void Event::reset ()
{
  ResetEvent (mHandle);
}

void Event::set ()
{
  SetEvent (mHandle);
}

void Event::pulse ()
{
  PulseEvent (mHandle);
}

Event::Event (bool start_state,bool manual)
  : SequenceObject (CreateEvent (NULL,manual,start_state,NULL))
{ }




