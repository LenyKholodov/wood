#include <math3d.h>
#include <iostream.h>

void     main ()
{
  vector3d_t v (1,2,3); v.normalize ();
  vector3d_t v1 (-1,3,2);
  float      angle = 3.14 / 2;

  quat_t     q  (v,angle);
  quat_t     q1 (v1,angle);

  matrix_t   d ;
  matrix_t   m = q.toMatrix ();
//  matrix_t   m (1);
//  matrix_t   m = matrix_t::translate (1,2,3);

  d [0] = float4 (-1, 0, 0, 0);
  d [1] = float4 ( 0, 0, 1, 0);
  d [2] = float4 ( 0, 1, 0, 0);
  d [3] = float4 ( 0, 0, 0, 1);

//  cout<<m<<endl;

//  m = d * m;

//  cout<<m<<endl;

  quat_t q2 = m;
  
  cout<<q<<endl;
  cout<<q1<<endl;
  cout<<q2<<endl;
}