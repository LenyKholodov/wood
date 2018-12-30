#define  DEBUG
#include <graph\matrix.h>

void     main ()
{
  vector3d_t v (5,5,5);
  vector3d_t x (1,1,1);

  matrix_t m (1);

  m  *=   matrix_t::translate (v*vector3d_t (2,2,2));
  m  *=   matrix_t::scale     (vector3d_t (2,2,2));
  m  *=   matrix_t::translate (-v);

  cout<<m*x<<endl;
}