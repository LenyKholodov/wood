#include <iostream.h>

class    Y
{
  public:
        Y () {}
        ~Y () { cout<<"KOKO!"<<endl; }        
};

void x ()
{
  throw 0;  
}

void    main ()
{
  try 
  {
    Y y;

    x ();
  }
  catch (int x)
  {
    cout<<x<<endl;
  }
}