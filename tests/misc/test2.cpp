#define  DEBUG
#include <misc.h>
#include <iostream.h>

void main ()
{
  static uint x [12345];

  memsetd (x,1234568,12345);

  cout<<x [12344]<<endl;
}
