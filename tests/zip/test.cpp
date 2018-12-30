#include <iostream.h>
#include <zip\unzip.h>

#pragma comment(lib,"zip.lib")

void     main ()
{
  unzFile file = unzOpen ("test.zip");
 
//  if (unzGoToFirstFile (file) != UNZ_OK)
//    cout<<"Error!"<<endl;

  if (unzOpenCurrentFile (file) != UNZ_OK)
    cout<<"Error at open!"<<endl;

  char buf [1200];

  cout<<unzReadCurrentFile (file,buf,20)<<endl;

  cout<<buf<<endl;

  unzClose (file);
}