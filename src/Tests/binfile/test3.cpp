#include <kernel.h>

void     main ()
{
  K_FastInit         ("test3.map");
  F_RegisterPackFile ("test_dir.zip");

  fileService->dump (dout);

  BinFile* file = F_OpenFile ("test2.txt",BF_OPEN_RO);

  if (!IsValid (file))
  {
    dout<<"Error"<<endl;
    return;
  }

  char buf [1024];

  file->read (buf,1024);

  buf [file->size ()] = 0;

  delete file;

  buf [1024] = 0;

  cout<<buf<<endl;  
}
