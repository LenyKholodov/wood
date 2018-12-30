#define DEBUG
#include <iostream.h>
#include <malloc.h>
#include <pool.h>

namespace memory
{
  void* operator new (size_t size)
  {
    dout<<"jeueje"<<endl;
    return NULL;
  }

  void  operator delete (void* x)
  {
    dout<<"koko"<<endl;
  }

  void solo ()
  {
    void* x = new char;
  }
};

void    main ()
{
  dout<<"in"<<endl;

  memory::solo ();

  char* x = new char;

  delete x;
}
