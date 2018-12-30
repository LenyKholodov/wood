#include <kernel.h>

void     main ()
{
  K_FastInit ("test5.map");

  Ring<int> list;

  list += 5;
  list += 6;
  list += 7;

//  list-=7;

  Ring<int>::diter iter   = list;
  int              count  = list.count ();

//  iter--;

  cout<<"count: "<<count<<endl;

  for (int i=0;i<count;i++,iter++)
    if (iter) 
      cout<<iter<<endl; 

  cout<<"exit"<<endl;
}