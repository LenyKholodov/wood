#define  DEBUG
#include <binfile\sbinfile.h>
#include <debug\debug.h>
#include <string.h>

SystemPool      sys;

void     main ()
{
  stdBinFile* file = new (&sys) stdBinFile (&sys,"test.dat",BinFile::OPEN_RW);

  static char test [] = "Hello world!";

  file->write (test,strlen(test));

  delete file;
}