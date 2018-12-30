#include <kernel.h>
#include <binfile\cachefile.h>

void     main ()
{
  K_FastInit ("test6.map");

  BinFile* file = F_OpenFile ("data.dat",BF_OPEN_CR|BF_OPEN_CACHE);

  static char string [] = "Hello world";  

  file->write (string,strlen(string));

  delete file;
}