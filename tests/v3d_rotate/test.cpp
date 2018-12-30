#define  DEBUG
#include <graph\vector3d.h>

void     main ()
{
  vector3d_t v0 (0);
  vector3d_t v1 (1,0,0);
  vector3d_t v2 (0,0,1);

  cout<<rotate (v0,v1,v2)<<endl;
}