#define  DEBUG
#include <console\vpool.h>

void     main ()
{
  FloatPool     pool (M_GetPool(),1024*1024);
  VarPool       vp   (&pool,128);

  int y = 3;

  vp.set ("x",12);
  vp.link ("y",&y);
  cout<<vp.geti ("x")<<endl;  
  cout<<vp.geti ("y")<<endl;  
  vp.set ("y",5);
  cout<<vp.geti ("y")<<endl;  
  cout<<vp.pos()<<endl;
}