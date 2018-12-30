#define DEBUG
#include <pool.h>
#include <memory\respool.h>

const    TEST_SIZE      = 1024*1024;

class   TestResource: public Resource
{
  public:
        TestResource (Cache* cache): Resource (cache) {}

        virtual      void* load () { return owner()->alloc (512,*this); }  
};

void     main ()
{
  ResourcePool cache  (M_GetPool(),1024*1024);
  ClasterCache cache1 (&cache,1024,1024*10),
               cache2 (&cache,64*1024,1024*1024);

  TestResource     res1   (&cache1);
  TestResource     res2   (&cache1);
  TestResource     res3   (&cache1);
  TestResource     res4   (&cache1);
  TestResource     res5   (&cache1);
  TestResource     res6   (&cache1);
  TestResource     res7   (&cache1);
  TestResource     res8   (&cache1);
  TestResource     res9   (&cache1);
  TestResource     res10  (&cache1);
  TestResource     res11  (&cache1);

  cout<<res1.lock ()<<endl;
  cout<<res2.lock ()<<endl;
  cout<<res3.lock ()<<endl;
  cout<<res4.lock ()<<endl;
  cout<<res5.lock ()<<endl;
  cout<<res6.lock ()<<endl;
  cout<<res7.lock ()<<endl;
  cout<<res8.lock ()<<endl;
  cout<<res9.lock ()<<endl;
  cout<<res10.lock ()<<endl;
  res1.unlock ();
  cout<<res11.lock ()<<endl;
}