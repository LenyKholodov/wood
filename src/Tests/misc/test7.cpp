#include <kernel.h>
#include <misc\queue.h>
#include <fixed\fixed.h>

void     main ()
{
  K_FastInit ("test7.map");

  Queue<int> queue;

  queue.push (5);
  queue.push (6);
  queue.push (7);

  cout<<queue.pop ()<<endl;
  cout<<queue.pop ()<<endl;
  cout<<queue.pop ()<<endl;
}