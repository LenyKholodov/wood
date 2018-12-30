#include <kernel\excpt.h>

void     main ()
{
  Str32  str ("hello world");
  Str256 s = str;

  cout<<s<<endl;
}