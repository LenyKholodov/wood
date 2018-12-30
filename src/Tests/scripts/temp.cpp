/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение данных для экземпляра типа Type
/////////////////////////////////////////////////////////////////////////////////////////////
    struct      taginfo_t
    {
      ReadValueFn       fn;
      void*             base;
      ostream&          err;
    };

class    TagStreamBuf: public streambuf
{
  public:
                TagStreamBuf (TextStreamReader& rdr,
                              streambuf&        buf                                                
                             ): mReader (rdr), mBuf (buf), mCall (0), mLast(0) {}

       virtual int   pbackfail (int i)  { dout<<"back"<<endl; return i; }
       virtual int   sync () { dout<<"sync"<<endl; return 0; }  //EOF
    virtual streampos seekoff(streamoff,ios::seek_dir,int)
    {
      dout<<"seekoff"<<endl;
      return 0;
    }
    virtual streampos seekpos(streampos,int =ios::in|ios::out)
    {
      dout<<"seek pos"<<endl;
      return 0;
    }
    virtual int xsputn(const char *,int)
    {
      dout<<"xputn"<<endl;
      return 0;
    }    
    virtual int xsgetn(char *,int)
    {
      dout<<"xgetn"<<endl;
      return 0;
    }
       virtual int   overflow   (int i) { dout<<"heye!"<<endl; return i;}
       virtual int   underflow  (void) {
           char c;
           mBuf.sgetn (&c,1);
           dout<<"xread: '"<<c<<"'"<<endl;
           mLast = c;
           if (mLast == '\n')
             mReader.mLine++;
           return mLast;  
       }

  private:
        TextStreamReader&   mReader;
        streambuf&          mBuf;
        int                 mCall, mLast;
};

////TEST

#include <kernel.h>

void     main ()
{
  K_FastInit (NULL);

  TextStreamReader::Reader rdr ("test2.dat");
  uint                     level = 0;

  for (char* str=rdr.next (level);str;str=rdr.next (level))
  {
    istream& is = rdr.stream ();
    
    cout<<str<<endl;

    while (is) 
    {
      char buf [128] = {0};
      is>>buf;
      cout<<"p: "<<buf<<endl;
      break;
    }
  }  
}

void TextStreamReader::ReadComments (istream& is,bool lineComment)
{
  if (lineComment)
    is.getline (mCurWord,mCurWord.size ());
  else
  {
    int  cmnt   = 0;
    char c      = 0;

    while (is)
    {
      is.get (c);  

      switch (c)
      {
        case '*':  cmnt++; break;
        case '/':  if (cmnt) cmnt++; break;
        default:   cmnt = 0;
      }
    }
  }    
}

void TextStreamReader::TraceTag     (istream& is)
{
  char c;
  int  cmnt = 0;

  while (is && c != open_tag && c != end_line)
  {
    is.get (c);  

    switch (c)
    {
      case '/':  cmnt++; break;
      case '*':  if (cmnt) cmnt++; break;
      default:   
        cmnt = 0;
    }  
  }

//Тэг может быть как блочным так и в одну строку

  if (c == end_line)
    return;

  int count = 1;

  while (is && count)
  {
    is.get (c);

    switch (c)
    {
      case '/':  cmnt++; break;
      case '*':  if (cmnt) cmnt++; break;
      case open_tag: count++; cmnt = 0; break;
      case close_tag: count--; cmnt = 0; break;
      default:   
        cmnt = 0;
    }  
  }
}