#include <kernel.h>

void     main ()
{
  D_Try
  {
    K_Init ();
    SrvMakeCurrent (D_Load ("test7.map"));
    
    BinFile* file = F_OpenFile ("bla",BF_OPEN_RW);

    file->write ("bla",4);

    delete file;

    FloatPool pool (M_GetPool (),1024);

    M_PushPool (&pool);

    void* x = new char [900];
    void* y = new char [90000];

    dout<<x<<" "<<y<<endl;

    dout<<"Hello world - 2"<<endl;

    D_Done ();
    K_Done ();

    dout<<"Hello world - 4"<<endl;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"LPLP!"<<endl;
  }

  dout<<"solo!"<<endl;
}