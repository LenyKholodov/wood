#define DEBUG
#include <binfile.h>

void     main ()
{
  F_RegisterPackFile ("test.zip","pak");
//  zipBinFile zip (M_GetPool(),"test.zip");  

//  F_Register ("pak",&zip);

  BinFile* file = F_OpenFile ("pak:\\test.txt",BF_OPEN_RO);

  static char     buffer [128];

  file->read (buffer,file->size ());

  buffer [file->size ()] = 0;

  cout<<buffer<<endl;

  delete file;
}
