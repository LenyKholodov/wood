#include "vec4.h"

using std::cout;
using std::endl;

const col4* color = NULL;

extern void test (const col4& c);

void main ()
{
  vec4 vec (1,0,0);
  col4 col (0,1,0);

  const vec4 n = cross (vec,col);

  test (n);
                   
//  cout<<col<<endl;
}

void test (const col4& c) 
{
  color = &c;
}