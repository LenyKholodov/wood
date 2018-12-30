#include <kernel.h>
#include <conio.h>
#include <engine\server.h>
#include <engine\bufstream.h>

#pragma comment (lib,"gamed.lib")

void     main ()
{
  K_FastInit (NULL);

  BufMessageStream s1 (M_GetPool (),1024),
                   s2 (M_GetPool (),1024);

  Connection       con (M_GetPool ());

  con.BindWriteStream (s1);
  con.BindReadStream  (s2);

  Server           srv (M_GetPool ());

  srv.AddConnection (con); 

  dout<<"end construct"<<endl;

  con.ws ().PutMessage (2,NULL,0);

  getch ();
  dout<<"exit"<<endl;
}