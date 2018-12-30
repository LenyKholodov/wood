#define  DEBUG
#include <console\command.h>

void     main ()
{
  FloatPool     pool (M_GetPool(),1024*1024);
  Command       cmd  (&pool,128);

  cmd.vpool.set ("x",100);

  cmd ("x 200");

  cout<<cmd.vpool.geti ("x")<<endl;  
}