#include <kernel.h>
#include <scripts\framerdr.h>

struct A
{
  int   x,y,z;

  static A* Create (A* a,FrameReader& rdr,void* self)
  {
    if (!a)
    {
      a = new A;

      if (!a)
        return NULL;

      dout<<"hey!"<<endl;

      rdr.BindValue ("x",a->x);  
      rdr.BindValue ("y",a->y); 
      rdr.BindValue ("z",a->z);

      return a;
    }

    cout<<"a.x\t"<<a->x<<endl;
    cout<<"a.y\t"<<a->y<<endl;
    cout<<"a.z\t"<<a->z<<endl;

    return a;           
  }
};

bool    read (istream& is,int i)
{
  is>>i;
  return false;
}

void     main ()
{        
  K_FastInit (NULL);
 
  FrameReader  rdr;

  int  x = 0,
       y = 0,
       z = 0;
  char str [128] = {0};
  A    a;
  double d [8] = {0.0};

  dout<<"x::: "<<&x<<endl;
  dout<<"y::: "<<&y<<endl;
  dout<<"z::: "<<&z<<endl;

  rdr.BindReader   ("f", read, &x);
  rdr.BindValue    ("x",     x);
  rdr.BindValue    ("y",     y);
  rdr.BindValue    ("z",     z);
  rdr.BindArray    ("d",     d); 
  rdr.BindString   ("c",     str);

  rdr.DeclareFrame ("a");
  rdr.BindCreater  ("a",     A::Create);

  rdr.ReadStream   ("test1.dat",cout);

  cout<<"X "<<x<<endl;
  cout<<"Y "<<y<<endl;
  cout<<"Z "<<z<<endl;

  cout<<"C "<<str<<endl;

  for (int i=0;i<8;i++)
    cout<<"d ["<<i<<"]\t"<<d [i]<<endl;
}
