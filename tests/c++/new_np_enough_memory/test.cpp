#include <iostream.h>
#include <list>

class    X
{
  public:
                X (int v): x(v) { cout<<"HUHU"<<endl; }

                void*  operator new (size_t) { return NULL; }

  private:
        int     x;
};

void    main ()
{
  try
  {
    X* x = new X (5);
    cout<<x<<endl;
  }
  catch (std::bad_alloc&) { cout<<"juju"<<endl; }    
}