#include <misc\map.h>
#include <kernel.h>

void     main ()
{
  K_FastInit ("test12.map");

  try
  {
    Map<int>      map;

    map.SetMode (Map<int>::CREATE);

    map [0] = 5;  
    map [1] = 15;  

    dout<<map [0]<<" "<<map [1]<<endl;
  }
  catch (Exception& exc)
  {
    dout<<exc<<endl;
  }  
}