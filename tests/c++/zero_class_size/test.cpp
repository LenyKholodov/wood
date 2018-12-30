#include <iostream.h>

#pragma pack (1)

class    Y
{
  public:
                Y (int v = 0): y (v) {}

                char y;
};

class    X
{
  public:
                X (Y* y,int v) { y->y = v; }
};

class   Z: public Y, public X
{
  public:
                Z (int y): Y (), X (this,y) {}
};

void    main ()
{
  Z     z (4);

  cout<<(int)z.y<<endl;
  cout<<sizeof (z)<<endl;
}