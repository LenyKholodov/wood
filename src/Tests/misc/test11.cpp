#include <kernel.h>
#include <misc\tree.h>
#include <time.h>

int      total = 10000;

void     TestDict ()
{
  Dict<int> tree;

  float start = clock ();

  for (int i=0;i<total;i++)
    tree.insert (i,i);  

  float end = clock ();

  cout<<"Dict fps: "<<float (total*CLK_TCK)/(end-start)<<endl;
}

void     TestTree ()
{
  BinTree<int> tree;

  float start = clock ();

  for (int i=0;i<total;i++)
    tree.insert (i,i);  

  float end = clock ();

  cout<<"Dict fps: "<<float (total*CLK_TCK)/(end-start)<<endl;
}

void main ()
{
  K_FastInit ("test11.map");

  
  TestDict ();
  TestTree ();
}