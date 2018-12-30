#include <math3d.h>

vector3d_t   ort (1,1,1);
float        ang = 3.14 / 3.0f;
vector3d_t   v (0,0,1);
matrix_t     m1 = matrix_t::rotate (vector3d_t (0,0,1),ang);
matrix_t     m2 = matrix_t::rotate (vector3d_t (1,0,0),3.0f*ang);

void     main ()
{
  quat_t        q1  (m1);
  quat_t        q2  (m2);
  vector3d_t    test (0,1,0);

  q1 *= q2;
  
  cout<<m1*m2*test<<" "<<q1.toMatrix ()*test<<endl;;
}
