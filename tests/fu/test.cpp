#define  DEBUG
#include <thread\unist\file.h>
#include <thread\thread.h>

SystemPool      sys;

DWORD           Init    (Thread*,DWORD);
DWORD           Process (Thread*,DWORD);

void     main ()
{
  Thread thread (&sys,Init,NULL);

  while (Thread::count ());
}

DWORD           Init    (Thread* thread,DWORD)
{
}

DWORD           Process (Thread*,DWORD)
{
}
