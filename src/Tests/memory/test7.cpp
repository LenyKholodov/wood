#define DEBUG
#include <pool.h>
#include <memory\respool.h>

class   TestResource: public Resource
{
  public:
        TestResource (Cache* cache): Resource (cache) {}

        virtual      void* load () { return alloc (256); }  
};

void     main ()
{
  ResourcePool cache  (M_GetPool(),1024*1024); cache.LockCache ();
  DefragCache  cache1 (&cache,1200);

  ClasterCache cache2 (&cache1,256,1024);
  DefragCache  cache3 (&cache1,1024);
  ResourcePool cache4 (&cache1,1024);

  dout<<"!!!: "<<&cache1<<" "<<&cache2<<" "<<&cache3<<endl;

  TestResource res1   (&cache2);
  TestResource res2   (&cache3);
  TestResource res3   (&cache4);

  dout<<"3"<<endl;
  cout<<res3.lock ()<<endl; res3.unlock ();
  dout<<"1"<<endl;
  cout<<res1.lock ()<<endl; res1.unlock ();
  dout<<"2"<<endl;
  cout<<res2.lock ()<<endl; res2.unlock ();
}
