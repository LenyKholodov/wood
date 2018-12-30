#define  DEBUG
#include <graph\matrix.h>

void     main ()
{
  vector3d_t fx (10,10,10);
//  vector3d_t fx (0);
  vector3d_t v = vector3d_t (0,0,1) + fx;

  matrix_t   m (1);

  m *= matrix_t::translate (fx);
  m *= matrix_t::rotate    (vector3d_t (0,1,0),3.14f/2.0f);
  m *= matrix_t::translate (-fx);

  cout<<m*v<<endl;
  cout<<matrix_t::rotate (vector3d_t (0,1,0),3.14f/2.0f) * (v-fx)+fx<<endl;;
}