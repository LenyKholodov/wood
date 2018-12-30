#include <iostream.h>

static char str [] = "Hello world";
static int i = 0;

class    Buffer: public streambuf
{
  public:
        virtual int     overflow (int i) { cout<<"koko"<<endl; return i; }
        virtual int     underflow () {
          static int call = 0;          
          call = !call;
          if (!call) return str [i++];         
          else return str [i];
        }
};

void    main ()
{
  Buffer buf;
  istream_withassign is (&buf);

  char str [128] = {0};
  is>>str;

  cout<<str<<endl;

  is>>str;

  cout<<str<<endl;
}