#include <iostream.h>

struct X
{
  union         
  {
    int x;
    int y;
    int z;
  };
};

void    main ()
{
  X x;

  x.x = 5;

  cout<<x.z<<endl;
}