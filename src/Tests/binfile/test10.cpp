#include <kernel.h>

void     main ()
{
  K_FastInit ("test10.map");

  F_SetCurrentDir ("test/");

  BinFile* file = F_OpenFile ("test1.txt",BF_OPEN_RO);

  if (!IsValid (file))
  {
    cout<<"Error at open file!"<<endl;
    return;
  }

  char  buffer  [128];

  file->read (buffer,file->size ());

  buffer [file->size ()] = 0;

  cout<<buffer<<endl;
  dout<<buffer<<endl;

  delete file;
}