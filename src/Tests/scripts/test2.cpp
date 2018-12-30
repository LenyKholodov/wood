#include <kernel.h>
#include <scripts\framewrt.h>

void     main ()
{
  K_FastInit ("test2.map");

  FrameWriter tout ("test5.dat");

  tout.OpenFrame ("bla");

  tout<<"Hello world"<<endl;

  tout.CloseFrame ();

  tout<<"Hello world"<<endl;
}