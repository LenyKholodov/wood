#define DEBUG
#include <ustring.h>

void     main ()
{
  str128 s;
  str128 s1 ("Hey-ho!");
  s = s + 1 + 2 + 3 + ' ' + 2.4 + ' ' + s1;
  s += " Hello world!";
  s1 = s;

  cout<<s<<endl;
  cout<<s1<<endl;;
}