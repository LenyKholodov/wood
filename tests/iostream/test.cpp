#include <iostream.h>
#include <time.h>

static   char string [] = "Hello world";
static   int  total = 0;
const __int64 TOTAL = 8*1024*1024;

class    buffer: public streambuf
{
  public:
                buffer () {                
                }

    virtual     int underflow () {
      if (total == TOTAL)
        return EOF;
      total++;
      return ' ';
    }

    virtual int overflow (int c)
    {
      cout<<"overflow"<<endl;
      return c;
    }

    streambuf* setbuf(char *, int)
    {
      cout<<"set buf"<<endl;
      return this;
    }

    streampos seekoff(streamoff, ios::seek_dir, int)
    {
      cout<<"seek off"<<endl;
      return 0;      
    }
};

void    main ()
{
  buffer             buf;
  istream_withassign is (&buf);

  double start = clock ();

  char c ;
  is>>c;

  double end  = clock ();    

  cout<<"FPS:\t"<<double (total)/((end-start)/double (CLK_TCK))<<endl;
  cout<<"total:\t"<<(int)total<<endl;
  cout<<"time:\t"<<(end-start)/double (CLK_TCK)<<endl;
}