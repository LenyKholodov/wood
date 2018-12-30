#include <misc\tree.h>
#include <kernel.h>

void     main ()
{
  K_FastInit ("test10.map");

  int i = 0;

  try
  {
    BinTree<int> tree;

    for (i=0;i<100;i++)
    {
      int x = random (100);
//      int x = i;

      BinTree<int>::diter iter = tree.find (x);

      if (iter.valid ())
      {
        dout<<"Повторение:\t"<<iter<<endl;
      }

      tree.insert (x,x);
    }

    int old = tree.count ();

    for (i=0;i<80;i++)
    {
      int x = random (100);
      tree.erase (x);
    }

    dout<<"Deleted:\t"<<old - tree.count ()<<endl;

//    tree.insert (5,5);
//    tree.insert (7,7);
//    tree.insert (1,1);

    BinTree<int>::diter iter  = tree.GetMin ();
    int                 count = tree.count ();

    dout<<"Count:\t"<<count<<endl;

    for (i=0;i<count;i++)
    {
      dout<<iter<<endl;;
      try
      {
        iter++;
      }
      catch (Exception& exc)
      {
        dout<<"!!!\t"<<exc<<endl;
      }  
    }  
  }
  catch (Exception& exc)    
  {
    dout<<"Error:\t"<<i<<endl;
    dout<<exc<<endl;
  }
}