#define  DEBUG
#include <mlist.h>
#include <iostream.h>

void     main ()
{
  int              array [128] = {0};
  MemListTmpl<int> list  (128,array);

  list.insert (1);
  list.insert (2);
  list.insert (3);
  list.insert (4);
  list.insert (5);

  MemListTmpl<int>::iterator iter = list;

  cout<<list.count ()<<endl;

  for (int i=0;i<list.count ()+2;i++,iter++)
    cout<<iter<<endl;
}
