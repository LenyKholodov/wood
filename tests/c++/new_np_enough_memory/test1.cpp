#include <iostream.h>

class    X
{
  public:
                void*   operator new (size_t size) { return ::operator new (size); }
                void    operator delete (void* x) { ::operator delete (x); }
};

void    main ()
{
  char* x = X::new char;
}
  