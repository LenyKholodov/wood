#include <thread.h>
#include <conio.h>

class    TestThread: public Thread
{
  public:
                TestThread (): Thread (::M_GetPool ()) {}
        virtual int process ();
};

void     main ()
{
  TestThread *t1 = new TestThread,
             *t2 = new TestThread;


  while (!kbhit ());

  delete t1;
  delete t2;

  dout<<"OLOLOL!"<<endl;
}

int TestThread::process ()
{
  dout<<"juju"<<endl;

  while (1)
  {
    dout<<id()<<endl;
  }

  return 0;
}