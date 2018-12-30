#include <kernel.h>

const    char writeMsg [] = "Zip test string with HHHHHHHHHHHHHHHHHHHHHHH";
const    char writeMsg1 [] = "Hello world";

static int offs = 0;

void     doWrite ()
{
  BinFile*   file = F_OpenFile ("test.pk",BF_OPEN_CR|BF_OPEN_PACK_STREAM4);  

  cout<<file->seek (10)<<endl;
  cout<<file->write (writeMsg,strlen (writeMsg))<<endl;  
  offs = file->getpos ();
  cout<<file->write (writeMsg1,strlen (writeMsg1))<<endl;  

  delete file;
}

void     doRead ()
{
  BinFile*   file = F_OpenFile ("test.pk",BF_OPEN_RO|BF_OPEN_PACK_STREAM);  

  char msg [1024] = {0};

  cout<<file->read (msg,90)<<endl;   msg [0] = 0;  
  cout<<file->seek (offs)<<endl;
  cout<<file->read (msg,90)<<endl;  

  cout<<"message is: '"<<msg<<"'"<<endl;

  delete file;
}

void     main ()
{
  K_FastInit ("test8.map");

  doWrite ();
  doRead  ();
}
