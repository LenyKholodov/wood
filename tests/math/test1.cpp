#include <math\matrix.h>        

void     main ()
{
  matrix_t m  = matrix_t::translate (vector3d_t (10,20,30)),
           m1 = matrix_t::translate (vector3d_t (40,50,60));

  matrix_t m2 = m * m1;

  cout<<m2<<endl;
}