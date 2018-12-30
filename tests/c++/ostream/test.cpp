#include <iostream.h>

class    Stream
{
  public:
};

Stream&  operator << (Stream& s,int f) { cout<<f<<'\t'; return s;}

template <class Type>
Stream&  operator<<  (Stream& s,const Type& t) {
  cout<<t<<'\t';
  return s;
}

void    main ()
{
  Stream        s;

  float         x = 5;
  int           y = 10;
  
  s<<x<<y;
}