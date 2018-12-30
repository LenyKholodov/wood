#include <string.h>
#include <iostream.h>

void     main ()
{
  char s1 [128] = "Hello",
       s2 [128] = "test simbols";

  strncpy (s2,s1,7);

  cout<<(int)s2[6]<<endl;
}