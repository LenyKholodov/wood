#include <iostream.h>
#include <string.h>
#include <strstrea.h>

void     main ()
{
  static char buf [] = "Hello world";

  strstreambuf  sBuf (buf,strlen (buf));

  istream_withassign is (&sBuf);

  cout<<"here! "<<is.rdstate ()<<endl;

  while (is)
  {
    char str [128] = {0};

    is>>str;    

    cout<<str<<endl;    

  }

  cout<<"here! "<<is.rdstate ()<<endl;

  sBuf.setbuf (buf,strlen (buf));
  sBuf.seekpos (0);
  is.clear    (ios::goodbit);

  cout<<"here! "<<is.rdstate ()<<endl;

  while (is)
  {
    char str [128] = {0};

    is>>str;    

    cout<<"::: "<<str<<endl;    
  }
}