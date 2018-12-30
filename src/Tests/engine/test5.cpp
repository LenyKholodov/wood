#include <engine\stream.h>
#include <iostream.h>
#include <time.h>
#include <kernel.h>

#pragma comment (lib,"gamed.lib")

const int TOTAL  = 1024*1024;

void     main ()
{
  K_FastInit (NULL);

  static char temp [1024*1024*16];
  const size_t size = 1024*1024*4;

  FlatMessageStream s (M_GetPool (),1024*1024*32);

  double start = clock ();

  for (int i=0;i<TOTAL;i++)
  {
    s.BeginWrite (123,NoObject);
    s.put        (temp,size);
    s.EndWrite   ();

    s.BeginRead ();
    s.EndRead   ();
  }

  double end  = clock ();

  cout<<"FPS:\t"<<double (CLK_TCK)*double (TOTAL)*double (size)/(end-start)<<endl;
  cout<<"FPS (at msg):\t"<<double (CLK_TCK)*double (TOTAL)/(end-start)<<endl;
}