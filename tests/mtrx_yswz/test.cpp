#define  DEBUG
#include <graph\matrix.h>
#include <misc.h>

void     main ()
{
  vector3d_t v (0,0,0);

  matrix_t m = matrix_t::translate (vector3d_t (10,0,0));

  for (int i=0;i<4;i++)
    swap (m[i][1],m[i][2]); 

  for (i=0;i<4;i++)
    swap (m[1][i],m[2][i]);

  cout<<m*v<<endl;
}