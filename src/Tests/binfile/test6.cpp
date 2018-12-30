#include <kernel.h>
#include <binfile\cbinfile.h>

void     main ()
{
  K_FastInit ("test6.map");

  BinFile* file = F_OpenFile ("data.dat",BF_OPEN_RO|BF_OPEN_CACHE);

  while (!file->eof ())
  {
    char c;
    file->read (&c,1);
    dout<<c;
  }  
  
  delete file;
}