#include <engine\bufstream.h>
#include <kernel.h>
#include <conio.h>

#pragma comment (lib,"gamed.lib")

void     main ()
{
  K_FastInit (NULL);

  BufMessageStream s (M_GetPool (),130);

  s.set (MessageStream::ST_FULL_WRITE);

  int total = 0;

  dout<<"Valid: "<<s.valid ()<<endl;

  while (!kbhit ())
  {
    s.LockWrite (-1);

    s<<"Hello world";
    s<<total++;

    s.UnlockWrite   ();

    s.LockRead ();

    int id = -1;
    char buf [128] = {0};

    s>>buf;
    s>>id; 

    cout<<id<<" "<<buf<<endl;  

    s.UnlockRead ();
  }

  cout<<"exit"<<endl;
}
