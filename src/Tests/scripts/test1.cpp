#define DEBUG
#include <scripts\us.h>

void     call ()
{
  cout<<"Call ok!"<<endl;
}

struct bla_t
{
  int x,y;
  char z [128];
}list [16];

void     prepare (USStreamReader* stream,uint mode,DWORD)
{
  if (mode == US_PRECALL)
  {
    static int pos = 0;
    bla_t&     bla = list [pos++];

    cout<<"rules!"<<endl;

    stream->Bind ("x",bla.x);
    stream->Bind ("y",bla.y);
    stream->BindString ("z",bla.z);
  }
}

void     main ()
{
  USStreamReader reader (M_GetPool(),cout);
  USStreamReader reader_loc (M_GetPool(),cout);

  RingTmpl<int>  ring;

  int   x = 0, y = 0;

  reader.Bind ("x",x);
  reader.Bind ("y",y);
  reader.BindRing ("z",ring);
  reader.BindCallFn ("func1",call);

  reader.BindReader ("bla_t",&reader_loc,prepare);

  reader.read ("test.dat");

  cout<<"X: "<<x<<" Y: "<<y<<endl;
  cout<<"X [1]: "<<list [1].x
      <<" Y [1]: "<<list [1].y
      <<" Z [1]: "<<list [1].z
      <<endl;

  RingTmpl<int>::iterator iter  = ring;
  int                     count = ring.count ();

  for (int i=0;i<count;i++,iter++)
    cout<<"Z ["<<i<<"]: "<<iter.data ()<<endl;
}