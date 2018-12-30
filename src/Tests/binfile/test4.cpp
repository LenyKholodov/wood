#include <default.h>

void     main ()
{
  K_FastInit ("test4.map");

//  F_RegisterPackFile ("todo.zip");
  F_RegisterPackFile ("test_dir.zip");

  SearchFile* s = F_FirstFile ("test*"); 
//  SearchFile* s = F_FirstFile ("todo"); 

  if (!s)
    return;
  
  do
  {
    cout<<s->name ()<<" "
        <<(size_t)s->size ()<<" "<<s->attr ()<<" ";
    cout<<systime_t (s->time (SearchFile::WRITE));
    cout<<endl;
  }  while (s->next ());
}

