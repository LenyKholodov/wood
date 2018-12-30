#include "quat.h"

using std::cout;
using std::endl;

void main ()
{ 
  EulerAnglef a (0,180,0);
  quatf       q  (a);
  vec3f       v  (1,0,0);
  matrix      m = toMatrix (q);

  cout<<normal (q*v)<<endl;
  cout<<normal (m*v)<<endl;
  cout<<equal (q,q)<<endl;
}
