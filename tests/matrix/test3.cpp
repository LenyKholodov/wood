#include <math3d.h>

void     main ()
{
  matrix_t m = matrix_t::translate (vector3d_t (12,23,34));

  cout<<m<<endl;
}