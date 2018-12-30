#define  DEBUG
#include <memory\memory.h>
#include <iostream.h>
#include <malloc.h>  

void     main ()
{
  cout<<M_Size ()<<endl;
  
  FloatPool pool (M_GetPool(),1000000);
  M_PushPool     (&pool);

  cout<<M_Size ()<<endl;

  M_PopPool      ();
}