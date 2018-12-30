#define  DEBUG
#include <misc.h>
#include <iostream.h>

void     main ()
{
  char src  [] = "Hey - ho!";
  char dest [128];

  cout<<upcase (src,dest)<<endl;
}