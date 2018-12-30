#include <default.h>

const int total = 100000;

void     main ()
{
  K_FastInit ("test13.map");

  F_RegisterPackFile ("todo.zip","pak");

  BinFile* file = NULL;

  float start = clock ();

  for (int i=0;i<total;i++)
  {
    file = F_OpenFile ("pak:\\todo",BF_OPEN_RO);    

    if (!IsValid (file))
    {
      cout<<"error "<<i<<endl;
      break; 
    }

    delete file;
  }

  float end = clock ();

  cout<<"FPS: "<<float (CLK_TCK*total) / (end-start)<<endl;
}