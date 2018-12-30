#define  DEBUG
#include <binfile.h>

void     main ()
{
  zipBinFile zip (M_GetPool(),"test.zip");

  BinFile* file = zip.open ("test.txt");

  if (!file)
  {
    dout<<"Error"<<endl;
    zip.dump (dout);
    return;
  }
  else
    cout<<zip.count ()<<endl;

  char buf [1024];

  file->read (buf,1024);

  delete file;

  buf [1024] = 0;

  dout<<buf<<endl;
}
