#define  DEBUG
#include <console\conbuf.h>

void     main ()
{
  BinFile*      file = F_OpenFile ("log",BinFile::OPEN_CR);
  ConsoleBuffer buf  (M_GetPool(),file,128,128);

  buf.os()<<"Hello world!"<<endl;

  char temp [128] = {0};

  cout<<"'"<<buf.string (0,temp)<<"'"<<endl;

  delete file;
}