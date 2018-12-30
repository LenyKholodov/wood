#define  DEBUG
#include <misc.h>
#include <kernel\units\memory.h>
#include <memory\memory.h>
#include <kernel\thread.h>

void     main ()
{
  Pool* x = M_GetPool();

  char str1 [256];
  char buf  [256];

  for (int i='€';i<='Ÿ';i++)
    str1 [i-'€'] = i;

  cout<<locase (str1,buf)<<endl;;

  for (i=' ';i<='¯';i++)
    str1 [i-' '] = i;
  for (i='à';i<='ï';i++)
    str1 [i-'à'+16] = i;

  cout<<upcase (str1,buf)<<endl;;
  cout<<upcase ("01234567890-=+`",buf)<<endl;
  cout<<locase ("01234567890-=+`",buf)<<endl;

  for (i='A';i<='Z';i++)
    str1 [i-'A'] = i;

  cout<<locase (str1,buf)<<endl;;  

  for (i='a';i<='z';i++)
    str1 [i-'z'] = i;

  cout<<upcase (str1,buf)<<endl;;  
}