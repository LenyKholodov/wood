#include <kernel.h>
#include "numbers.h"
#include "numbers.inl"

Pool* Number::mNumbersPool = NULL;

void     main ()
{
  K_FastInit ("test3.map");

  Number::mNumbersPool = M_GetPool ();

  Float a = 5, b = 6;
  Integer x = 2;
  Number& c = a - b ;
  cout<<c<<endl;
}