#define DEBUG
#include <pool.h>
#include <memory\swap.h>

void     main ()
{
  SwapFile sfile (M_GetPool(),"swap.wd");

  char str  [2048] = "Hello world from swap file and from byte header correction",
       str1 [2048] = {0};

  long uid1 = sfile.write (str,512);
  sfile.write (str,512);
  long uid2 = sfile.write (str,512);
  sfile.read (str1,uid1);
  sfile.read (str1,uid2);

  long offs = 1024 - strlen (str) - 1;

  strcpy (str+offs,str);

  sfile.read (str1,sfile.write (str,1024+512));

  cout<<str1+offs<<endl;
}