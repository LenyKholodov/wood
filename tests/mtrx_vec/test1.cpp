#define  DEBUG
#include <graph\matrix.h>

void     main ()
{
  vector3d_t offs (10,10,10);
  vector3d_t ox   (0,1,0);
  vector3d_t oy   (-1,0,0);
  vector3d_t oz   (0,0,1);

  vector3d_t v    (1); //10-1,10+1,10+1  

  matrix_t   m (1);

  memcpy (m[0],&ox,sizeof (vector3d_t));
  memcpy (m[1],&oy,sizeof (vector3d_t));
  memcpy (m[2],&oz,sizeof (vector3d_t));

  matrix_t  x (1);

  x *= m;
  x *= matrix_t::translate (-offs);

  cout<<x * v<<endl;
}