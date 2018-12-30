#include <iostream.h>

void     test (float x [])
{
  cout<<x [0]<<endl;
}

void    main ()
{
  float x [100]  = {0};

  test (&x [0]);
}