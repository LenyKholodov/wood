#define  DEBUG
#include <graph\grtypes.h>
#include <iostream.h>

const    float  EPS = 0.1;

BOOL     cross (const line_t*,vertex_t&);

void     main ()
{
  line_t        l [2] = {{1,4,2},{0,1,3}};
  vertex_t      v;

  cout<<cross (l,v)<<endl;
  cout<<v.x<<" "<<v.y<<endl;
}

BOOL     cross (const line_t* l,vertex_t& res)
{
  if (fabs (l [1].a) > EPS && !(fabs (l [0].b) < EPS && fabs (l [1].b) < EPS ))
  {
    float k = -l [0].a / l [1].a;
    
    res.z   = 0;
    res.y   = -(l [0].c + l [1].c * k) / (l [0].b + l [1].b * k);
    res.x   = -(l [1].c + l [1].b * res.y) / l [1].a;
  }
  else if (fabs (l [1].b) > EPS && !(fabs (l [0].a) < EPS && fabs (l [1].a) < EPS ))
  {
    float k = -l [0].b / l [1].b;

    res.z   = 0;
    res.x   = -(l [0].c + l [1].c * k) / (l [0].a + l [1].a * k);
    res.y   = -(l [1].c + l [1].a * res.x) / l [1].b;
  }
  else 
    return FALSE;

  return TRUE;
}
