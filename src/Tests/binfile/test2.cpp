#include <default.h>

void     main ()
{
  K_FastInit    ("test4.map");
  F_AddFastPath ("textures","test\\");

  BinFile* file = F_OpenFile ("textures","todo",BF_OPEN_RO);

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

  dout<<buf<<endl;  
}
