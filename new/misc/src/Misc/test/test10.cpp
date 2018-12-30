#include <misc\tree.h>
#include <kernel.h>

void     main ()
{
  K_FastInit ("test10.map");

  try
  {
    BinTree<int> tree;

    tree.insert (5,5);
    tree.insert (7,7);
    tree.insert (1,1);

    BinTree<int>::diter iter = tree.GetLast ();

    dout<<"solo"<<endl;

    for (int i=0;i<10;iter--,i++)
      dout<<iter<<endl;;
  }
  catch (Exception& exc)    
  {
    dout<<exc<<endl;
  }
}