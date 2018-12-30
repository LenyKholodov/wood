#include "matrix.h"
#include "vec4.h"
#include "vec3.h"
#include <time.h>

const int total = 1000000;

using std::cout;
using std::endl;

float x=0;

void main ()
{
  matrix4f m = 1;
  matrix4f m1 = 2;
  vec3f v (1,0,0);
  vec3f v1 (0,1,0);

  clock_t start = clock ();

  for (int i=0;i<total;i++)
    m*=m1;

  clock_t end = clock ();
        
  cout<<m<<endl;
  cout<<"FPS:\t"<<float (total)/float (end-start)*float (CLK_TCK)<<endl;
}