#include <default.h>
#include <resource\caches.h>
#include "testres.h"

void    main ()
{
  D_BindMapFile ("test1.map");

  DefragCache cache (M_GetPool(),1001*1024);
  TestResource res1 (300*1024),
               res2 (300*1024),
               res3 (300*1024),
               res4 (400*1024);

  res1.SetOwner (&cache);
  res2.SetOwner (&cache);
  res3.SetOwner (&cache);
  res4.SetOwner (&cache);

  dout<<&res1<<" "<<res1.lock ()<<endl;
  dout<<&res2<<" "<<res2.lock ()<<endl;
  dout<<&res3<<" "<<res3.lock ()<<endl;
  
  res2.unlock ();
  res3.unlock ();

  res3.lock   ();
  res3.unlock ();

  dout<<&res4<<" "<<res4.lock ()<<endl;

  res1.check ();
  res2.check ();
  res3.check ();
  res4.check ();

  dout<<"End main"<<endl;
}
