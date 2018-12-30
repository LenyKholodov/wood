#define  DEBUG
#include <graph\matrix.h>

void     main ()
{
  matrix_t m (1);
  vector3d_t v (1,1,1);
  vector3d_t x (6,4,5);

  m [0][0] = 0;  m [0][1] = -1; m [0][2] = 0;
  m [1][0] = 1;  m [1][1] =  0; m [1][2] = 0;
  m [2][0] = 0;  m [2][1] =  0; m [2][2] = 1;
  m [3][0] = -5;  m [3][1] =  -5; m [3][2] = -5;  

  cout<<normalize (m*x)<<endl;  
}