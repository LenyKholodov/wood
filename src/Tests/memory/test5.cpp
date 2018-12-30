#define DEBUG
#include <pool.h>
#include <memory\respool.h>

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

  cout<<res1.lock ()<<endl; res1.unlock ();
  cout<<res2.lock ()<<endl; res2.unlock ();
  cout<<res1.lock ()<<endl; res1.unlock ();
}