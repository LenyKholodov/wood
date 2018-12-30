#include <kernel.h>
#include <scripts\framerdr.h>

struct  A
{
  int   x, y;
  int   l,m;
  float* arr;

        struct B {
          int x,y;
              B (): x(0), y(0) {}
        }b;

  A (): x (0), y(0), l(0), m(0), arr(NULL) {}

  static A& create (uint act,istream& is,A& a,void*,void*)
  {
    if (act == FrameReader::act_create)
    {
      dout<<"create a! "<<&a<<endl;
      is>>a.l>>a.m;
      dout<<"create ok"<<endl;
      a.arr = new float [16];
    }

    return a;
  }
};

void     main ()
{
  K_FastInit (NULL);
 
  FrameReader  rdr;

  int  x = 0,
       y = 0,
       z = 0;
  double d [8] = {0.0};
  char   c [128] = {0};
  A  a;

  rdr.BindValue    ("x",     x);
  rdr.BindValue    ("y",     y);
  rdr.BindValue    ("z",     z);
  rdr.BindArray    ("d",     d);
  rdr.BindString   ("c",     c);

  rdr.BindTagValue ("a",     a,  A::create); //abs, because owner = root
  rdr.BindTagValue ("a.b",   toNull(A).b);

  rdr.BindValue    ("a.x",   toNull(A).x);
  rdr.BindValue    ("a.y",   toNull(A).y);

  rdr.BindArrayPtr ("a.arr", toNull(A).arr); //!!!

  rdr.BindValue    ("a.b.x", toNull(A::B).x);
  rdr.BindValue    ("a.b.y", toNull(A::B).y);

  rdr.ReadStream   ("test1.dat",cout);

  cout<<"End read"<<endl;
  dout<<"End read"<<endl;

  cout<<"X "<<x<<endl;
  cout<<"Y "<<y<<endl;
  cout<<"Z "<<z<<endl;

  cout<<"C "<<c<<endl;

  for (int i=0;i<8;i++)
    cout<<"d ["<<i<<"]\t"<<d [i]<<endl;

  for (i=0;i<4;i++)
    cout<<"a.arr ["<<i<<"]\t"<<a.arr [i]<<endl;


  cout<<"a.l "<<a.l<<endl;
  cout<<"a.m "<<a.m<<endl;

  cout<<"a.x "<<a.x<<endl;
  cout<<"a.y "<<a.y<<endl;

  cout<<"a.b.x "<<a.b.x<<endl;
  cout<<"a.b.y "<<a.b.y<<endl;
}