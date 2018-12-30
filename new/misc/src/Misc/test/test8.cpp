#include <kernel.h>
#include <misc\stack.h>

void     main ()
{
  K_FastInit ("test8.map");

  FixedStack<int> queue (12);

  queue.push (5);
  queue.push (6);
  queue.push (7);

  cout<<queue.pop ()<<endl;
  cout<<queue.pop ()<<endl;
  cout<<queue.pop ()<<endl;
}