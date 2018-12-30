#include <kernel.h>

void     main ()
{
  K_FastInit ("test5.map");

  try
  {
    List<int> list;

    list += 5;
    list += 6;

    List<int>::diter iter   = list;
    int              count  = list.count ();

    cout<<"count: "<<count<<endl;

    iter--;

    for (int i=0;i<count;i++,iter++)
      if (list.IsValid (iter)) 
        cout<<iter<<endl; 

    cout<<"exit"<<endl;
  }
  catch (Exception& exc)
  {
    cout<<exc<<endl;
  }
}