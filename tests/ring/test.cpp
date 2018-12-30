#define DEBUG
#include <pool.h>
#include <ring.h>

typedef RingTmpl<int> IntRing;

void     main ()
{
  IntRing ring;

  ring.insert (1);
  ring.insert (2);

  IntRing::iterator iter = ring;

  for (int i=0;i<2;i++,iter++)
    cout<<iter.data()<<endl;    
}