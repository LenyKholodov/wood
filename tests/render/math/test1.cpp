#include "vec3.h"

using std::cout;
using std::endl;

void test (const vec3f& c) 
{
  cout<<c<<endl;
}

vec3f vec (1.4,0.5,0);
vec3f col (0,1,0.8);

void main ()
{
  const vec3f n = cross (vec,col);

  test (n);
                   
  cout<<col<<endl;
}
