#define  DEBUG
#include <graph\grtypes.h>
#include <graph\matrix.h>
#include <debug.h>

void     main ()
{
  vertex_t  v (0,0,0);
  matrix_t  m = matrix_t::translate (vector3d_t (1,2,3) );

  v = m * v;

  dout<<v<<endl<<endl;
  dout<<m<<endl;
}