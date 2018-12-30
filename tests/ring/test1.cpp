#include <kernel.h>

void     main ()
{
  K_FastInit (NULL);

  Ring<int> ring;

  ring.insert (1);
  ring.insert (2);

  Ring<int>::iterator iter = ring;

  for (int i=0;i<2;i++,iter++)
    cout<<iter.data()<<endl;    
}