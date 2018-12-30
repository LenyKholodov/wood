#include <kernel.h>

void     main ()
{
  K_FastInit ("test7.map");

  zipBinFile zip ("test.zip");

  SearchFile* s = zip.OpenSearch ("*");

  do 
    cout<<s->name ()<<" "<<s->size ()<<endl;
  while (s->next ());
}