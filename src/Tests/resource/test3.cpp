#include <default.h>
#include <resource\caches.h>
#include "testres.h"

void    main ()
{
  D_BindMapFile ("test1.map");

  ClasterCache cache (M_GetPool(),1024*1024,400*1024);
  TestResource res1 (350*1024),
               res2 (350*1024),
               res3 (350*1024);

  res1.SetOwner (&cache);
  res2.SetOwner (&cache);
  res3.SetOwner (&cache);

  dout<<&res1<<" "<<res1.lock ()<<endl;

  if (res1.buf) strcpy ((char*)res1.buf,"Hello world - 1");

  dout<<&res2<<" "<<res2.lock ()<<endl; 

  if (res2.buf); strcpy ((char*)res2.buf,"Hello world - 2");

//res1.unlock *();
  dout<<&res3<<" "<<res3.lock ()<<endl;
  
  dout<<"End main"<<endl;
}
