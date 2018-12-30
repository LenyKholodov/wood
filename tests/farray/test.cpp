#define  DEBUG
#include <farray.h>

SystemPool              sys;
Debug                   debug (&sys);
LumpListTmpl<int>       list (&sys,1024);

void     main ()
{  
  LumpListTmpl<int>::iterator iter = list;

  for (int i=0;i<4000;i++,iter++)
    iter=i;

  for (;i>=0;i--,iter--)
    dout<<iter<<endl;

  dout<<"Hey-ho!"<<endl;
}