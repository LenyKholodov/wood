#include <engine\bufstream.h>
#include <time.h>
#include <kernel.h>
#include <conio.h>

#pragma comment (lib,"gamed.lib")

static bool     go = false;

class    WriteThread: public Thread
{
  public:
                WriteThread (): Thread (false) { StartThread (); }
        virtual int process ();
};

class    ReadThread: public Thread
{
  public:
                ReadThread (): Thread (false) { StartThread (); }
        virtual int process ();
};

BufMessageStream* s = NULL; 
int               wcount = 0;
int               rcount = 0;

void     main ()
{
  K_FastInit (NULL);

  s = new BufMessageStream (M_GetPool (),28);

  s->set (MessageStream::ST_READ_WAIT);
  s->set (MessageStream::ST_WRITE_WAIT);
//  s->set (MessageStream::ST_FULL_WRITE);

//  Thread* t [2] = {new WriteThread, new ReadThread ()};
//  Thread* t [4] = {new WriteThread, new WriteThread, new ReadThread, new ReadThread ()};

  const int THREADS_COUNT = 128;
  Thread* t [THREADS_COUNT] = {NULL};

  dout<<"sizes "<<sizeof (WriteThread)<<" "<<M_GetPool ()->size ()<<endl;

  for (int i=0;i<THREADS_COUNT>>1;i++)
  {
    t [i] = new WriteThread;
    dout<<"write: "<<t [i]<<" "<<clock ()<<endl;
//    Sleep (5);
  }  

  for (;i<THREADS_COUNT;i++)
  {
    t [i] = new ReadThread;
    dout<<"read: "<<t [i]<<" "<<clock ()<<endl;
//    Sleep (5);
  }  

  dout<<"all create"<<endl;

  go = true;

  dout<<"go "<<clock ()<<endl;

  double start = clock ();
  
  getch ();  

  go = false;

  double end = clock ();

  dout<<"exit "<<clock ()<<endl;
  dout<<"FPS:\t"<<double (rcount+wcount)/(end-start)*double (CLK_TCK)<<endl;

  while (K_GetCurrentThread ()->count ());
}

int WriteThread::process ()
{
  while (!go);

//  dout<<"Begin write: "<<id ()<<endl;

  static int total = 0;

  while (go)
  {
    s->LockWrite (-1);
    clock_t time = clock ();
    (*s)<<total++<<id ()<<time;
//    dout<<"write\t"<<total-1<<"\t\tfrom\t"<<id ()<<"\t\t\t\t\t\tat time\t"<<time<<endl;
    s->UnlockWrite ();

    wcount++;
  }

  return 0;
}

int ReadThread::process ()
{
  while (!go);

//  dout<<"Begin read: "<<id ()<<endl;

  while (go)
  {
    if (s->LockRead ())
    {
      int     id    = -1; 
      clock_t time  = 0, cur = clock ();
      int     total = 0;

      (*s)>>total>>id>>time;

//      dout<<"read\t"<<total<<"\twrite thread\t"<<id<<"\tat time\t"<<time
//          <<"\tfrom thread\t"<<this->id ()<<"\tat time\t"<<cur
//          <<"\tdelta\t"<<(cur-time)
//          <<endl;

      s->UnlockRead ();

      rcount++;
    }
  }

  return 0;
}
