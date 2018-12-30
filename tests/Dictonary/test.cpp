#define  DEBUG
#include <dict.h>

int      random (int max) { return rand() % max; }

typedef DictionaryTmpl<long>    Dict;

void     main ()
{
  SystemPool    sys;    
  Dict          tree (&sys);

  while (1)
  {
    int code = random (3);
    
    switch (code)
    {
      default:
      case 0:
      {
        long x = random(65536);

        tree.insert (x,x);

        break;
      }
      case 1:
      {
        tree.erase (random(65536));

        break;
      }          
      case 2:
      {
        long x = random(65536);        
 
        if (tree.present(x) && tree.search (x) != x)
          dout<<"Error: for index: "<<x<<" data: "<<tree.search (x)<<endl;

        break;
      }
    }
  }  
}
