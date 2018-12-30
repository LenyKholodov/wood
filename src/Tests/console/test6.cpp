#define  DEBUG
#include <console\vpool.h>

void     main ()
{
  FloatPool     pool (M_GetPool(),1024*1024);
  VarPool       vp   (&pool,128);

//  char x [120];

  vp.set ("x","123");
  vp.set ("x","bla-bla!");

  cout<<vp.gets ("x")<<endl;  
}