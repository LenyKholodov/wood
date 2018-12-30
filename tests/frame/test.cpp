#define DEBUG
#include <graph\grtypes.h>
#include <graph\matrix.h>
#include <debug.h>

vector3d_t   tri [3] = {
  vector3d_t (-10,10,-500),
  vector3d_t (-100,-100,5000),
  vector3d_t (-100000,-1000,15)
};

vector3d_t   n = normalize ((tri [1] - tri [0]) ^ (tri [2] - tri [0]));

void         convert ();

void     main ()
{
  dout<<tri [0]<<" "<<tri [1]<<" "<<tri [2]<<endl;

  convert ();

  dout<<tri [0]<<" "<<tri [1]<<" "<<tri [2]<<endl;
}

void         convert ()
{
  vector3d_t  ort   = normalize (vector3d_t (0,0,1) ^ n);
  float       _cos  = n & vector3d_t (0,0,1);
//  float       ang   = _cos >= 0 ? acos (_cos) : 3.14f/2.0f + acos (fabs (_cos));
  float       ang   = acos (_cos);
  matrix_t    trans = matrix_t::rotate (ort,-ang);

  dout<<ort<<endl;
  dout<<_cos<<" "<<ang*57.3<<endl;
  dout<<n<<" "<<normalize (trans * n)<<endl;  

  for (int i=0;i<3;i++)
    tri [i] = trans * tri [i];
}
