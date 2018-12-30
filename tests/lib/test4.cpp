#define  DEBUG
#include <debug.h>

void     main ()
{
  D_Try
  {
    void* x = NULL;

    memset (x,0,1000000);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    cout<<"Error in block"<<endl;
  }
  cout<<"continue"<<endl;
}