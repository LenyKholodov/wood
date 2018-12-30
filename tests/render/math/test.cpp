#include "vec3.h"
#include <time.h>
#include <iostream>

const int total = 10000000;

vec3 v1, v2,v3;

using std::ostream;
using std::endl;
using std::cout;

inline void add (float* a,float* b)
{
  for (int i=0;i<3;i++)
    a [i] += b [i]; 
}

inline void mul (float* a,float* b)
{
  for (int i=0;i<3;i++)
    a [i] *= b [i]; 
}

inline void mul1 (float* a,float* b,float* c)
{
  for (int i=0;i<3;i++)
    c [i] = a [i] * b [i]; 
}

inline void copy (float* dest,float* src)
{
  for (int i=0;i<3;i++)
    dest [i] = src [i];
}

inline float dot (float* a,float* b)
{
  float res = 0;

  for (int i=0;i<3;i++)
    res += a [i] * b [i];

  return res;
}

float res;

void main ()
{
  float *a = v1, *b = v2, *c = v3;

  clock_t start = clock ();

  for (int j=total;j--;)
  {
    mul1 (a,b,c);
    mul1 (c,a,b);
    mul1 (b,c,a);
  }

  clock_t end = clock ();

  cout<<"FPS:\t"<<float (total)/float (end-start)*float (3*CLK_TCK)<<endl;

  start = clock ();

  for (j=total;j--;)
  {
    v3 = v1 * v2;
    v2 = v3 * v1;
    v1 = v2 * v3;
  }

  end = clock ();

  cout<<"FPS:\t"<<float (total)/float (end-start)*float (3*CLK_TCK)<<endl;
}        
