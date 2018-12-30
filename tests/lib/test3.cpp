#define  DEBUG
#include <thread\thread.h>

DWORD           Process         (Thread*,DWORD);
DWORD           Init            (Thread*,DWORD);

void     main ()
{
  SystemPool    sys;
  Thread        thread  (&sys,1,Init,NULL);
  Thread        thread1 (&sys,2,Init,&thread);  

  while (Thread::GetActiveThreads() != 0);
}

DWORD           Init            (Thread* thread,DWORD value)
{
  cout<<"Init thread: "<<thread->id()<<" with value: "<<value<<endl;

  thread->SetFn (THREAD_PROCESS,Process);

  return THREAD_CONSTRUCT_OK;
}

DWORD           Process         (Thread* thread,DWORD value)
{
  cout<<"Process thread "<<thread->id()<<" with value "<<value<<endl;

  for (int i=0;i<10000000;i++);

  cout<<"Out from process thread "<<thread->id()<<" with value "<<value<<endl;

  return THREAD_FIN_OK;
}