#include <engine\bufstream.h>
#include <kernel.h>

#pragma comment (lib,"gamed.lib")

void     main ()
{
  K_FastInit (NULL);

  BufMessageStream s (M_GetPool (),231);

  char buf [128] = {0};
  msg_t msg = -1;

  cout<<s.PutMessage (-1,buf,128)<<endl;
  cout<<s.PutMessage (-1,buf,128)<<endl;
  cout<<s.PutMessage (-1,buf,128)<<endl;
  cout<<s.GetMessage (msg,buf,128)<<endl;
  cout<<s.PutMessage (-1,buf,128)<<endl;
}