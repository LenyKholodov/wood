#define DEBUG
#include <time.h>
#include <pool.h>
#include <memory\respool.h>

const   int     TOTAL   = 1000000;

class   TestResource: public Resource
{
  public:
        TestResource (Cache* cache): Resource (cache) {}

        virtual      void* load () { return owner()->alloc (256,*this); }  
};

void     main ()
{
  ResourcePool cache  (M_GetPool(),1024*1024);
  ClasterCache cache1 (&cache,1024,1024),
               cache2 (&cache1,256,1024),
               cache3 (&cache1,256,1024);

  TestResource     res1   (&cache2);
  TestResource     res2   (&cache3);

  float start = clock ();

  for (int i=0;i<TOTAL;i++)
  {
    res1.lock (); res1.unlock ();
    res2.lock (); res2.unlock ();
  }

  float end   = clock ();

  cout<<"FPS: "<<float (2*TOTAL*CLK_TCK)/(end-start)<<endl;
}
