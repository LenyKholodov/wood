#include <kernel.h>
#include <stdio.h>

void     main ()
{
  K_FastInit ("test1.map");

  BinFile* file = F_OpenFile ("todo",BF_OPEN_RO);

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