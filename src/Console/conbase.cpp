#include <pch.h>
#include <console\conbase.h>

/////////////////////implementation of iostream utils///////////////////////////

class ioconsolebuf: public streambuf
{
  public:
                  ioconsolebuf  (ConsoleBase&,int linelen);
    virtual       ~ioconsolebuf ();

    virtual int   overflow   (int d);  
    virtual int   underflow  (void);  
            void  next       () { mBuf [mPos] = 0; mConBase.message (mBuf); mPos = 0; }

  private:
     ConsoleBase&       mConBase;  
     int                mPos, mCount;
     char*              mBuf;     
};

int ioconsolebuf::overflow (int simb)
{
  switch (simb)
  {
    case '\n':
      next ();      
      break;
    case '\r':
      mPos = 0;
      break;  
    case 8:
      if (mPos) mPos--;      
      break;  
    case '\t':
      if (mPos+8<mCount) 
        for (int i=0;i<8;i++,mPos++)
          mBuf [mPos] = ' ';
      else               
        for (;mPos<mCount;mPos++)
          mBuf [mPos] = ' ';
      break;
    default:
      if (mPos != mCount)
      {
        mBuf [mPos++] = simb;
        break; 
      }  
  }

  return simb;
}

int ioconsolebuf::underflow()  
{
  return EOF;
}

ioconsolebuf::ioconsolebuf  (ConsoleBase& buf,int linelen)
             : streambuf  (), 
               mConBase   (buf),
               mCount     (linelen),
               mPos       (0),
               mBuf       (NULL) 
{
  unbuffered(1);   

  mBuf = (char*)mConBase.GetPool()->alloc (linelen);

  if (!mBuf)
  {
    D_Message ("ioconbuf: No enough memory for sys data");
    return;
  }
}

ioconsolebuf::~ioconsolebuf ()
{
  if (mBuf) mConBase.GetPool()->free (mBuf);
}

class ioconsolebase: public virtual ios
{
  public:
    ioconsolebase (ConsoleBase& _buf,int llen):buf (_buf,llen) { ios::init (&this->buf); }
    ioconsolebuf buf;
};

class ioconsolestream: public MemObject, public ioconsolebase, public ostream
{
  public:
    ioconsolestream (Pool* pool,ConsoleBase& _buf,int llen)
      : MemObject (pool), ioconsolebase(_buf,llen),ostream (&buf) {}
};

//////////////////////////implementation of class ConsoleBase///////////////////

ostream&  ConsoleBase::os ()
{ 
  return *mOs;           
}

void   ConsoleBase::ScrollDown       (int offs)
{
  if (mTop + mHeight + offs - 1 <= mBuf->pos ())
  {
    mTop += offs; 
    if (mAutoRedraw) refresh (); 
  }  
}

void   ConsoleBase::ScrollUp         (int offs)
{
  if (mTop - offs >= 0)
  {
    mTop -= offs; 
    if (mAutoRedraw)  refresh (); 
  }  
}

void   ConsoleBase::top           (int top)
{
  if (top + mHeight <= count () && top >= 0)
  {
    mTop = top;    
    if (mAutoRedraw) refresh (); 
  }  
}

BOOL   ConsoleBase::message   (const char* msg)
{
  BOOL fmt  = FALSE;
  uint last = mComLine ? mHeight - 1 : mHeight;
  
  if (mBuf->pos () >= last && mTop+last == mBuf->pos())
    fmt = TRUE;

  if (!mBuf->message (msg))  return FALSE;
  else 
  {
    if (fmt) mTop = mBuf->pos()-last;

    if (mAutoRedraw) 
    {
      if (mTop + mHeight < mBuf->pos ())
        mTop = mBuf->pos () - mHeight;
      refresh ();
    }      

    return TRUE;
  }
}

void  ConsoleBase::refresh   (BOOL state)
{
  mAutoRedraw = state;
}

void   ConsoleBase::refresh   ()
{
  D_Try
  {
    if (mGrConsole)
    {
      mGrConsole->begin ();

      if (!mComLine)
        for (int i=0;i<mHeight;printf (i,mBuf->string (i+mTop,mTempBuf),CJ_LEFT),i++);
      else
      {
        for (int i=0;i<mHeight-1;printf (i,mBuf->string (i+mTop,mTempBuf),CJ_LEFT),i++);

        char* outStr = mComLine->gets ();
        int   len    = strlen (outStr);

        if (mComLine->cursor())
        {
          static char buf [4096];

          if (mComLine->pos () > 4000)
            D_Message ("ConsoleBase: Command line cursor is fucker runner! Stop it!")
          else if (len - mWidth <= mComLine->pos ())
          {
            memset (buf,' ',mComLine->pos ());

            buf [mComLine->pos ()]   = '_';
            buf [mComLine->pos ()+1] = 0;

            printf (mHeight-1,buf,CJ_RIGHT);
          }            
        }  

        printf (mHeight-1,outStr,CJ_RIGHT);
      }

      mGrConsole->end ();
    }  
  }
  D_Exc ("ConsoleBase: Error at refresh");  
}

void    ConsoleBase::printf (int line,const char* msg,int justify)
{
  D_Try
  {
    if (!msg)
      return;

    int len = strlen (msg);

    switch (justify)
    {
      case CJ_LEFT:
        if (len >= mWidth)
        {
          char old = msg [mWidth-1];
          ((char*)msg)[mWidth-1] = 0; 
          mGrConsole->writeln (line,msg);
          ((char*)msg)[mWidth-1] = old;
        }  
        else
          mGrConsole->writeln (line,msg);        
        break;
      case CJ_RIGHT:
        if (len >= mWidth)
        {
          int first = len - mWidth;
          mGrConsole->writeln (line,msg+first);
        }  
        else
          mGrConsole->writeln (line,msg);
        break;
    }
  }
  D_Exc ("ConsoleBase: Error at printf");
}

ConsoleBase::ConsoleBase (Pool* _Pool,const char* log,int _LineNum,int _LineLen)
            : MemObject (_Pool),
              mFile     (F_OpenFile (log,BF_OPEN_CR)),
              mBuf      (NULL),
              mWidth    (DEFAULT_CONSOLE_WIDTH),
              mHeight   (DEFAULT_CONSOLE_HEIGHT),
              mTop      (0),
              mTempBuf  (NULL),
              mGrConsole(NULL),
              mComLine  (NULL),
              mAutoRedraw (FALSE),
              mOs         (NULL)
{
//  D_Try
//  {
    mTempBuf = (char*)alloc (_LineLen);
    mBuf     = ext_new ConsoleBuffer (GetPool(),mFile,_LineNum,_LineLen);
    mOs      = ext_new ioconsolestream (GetPool(),*this,_LineLen);

    if (!mTempBuf || !IsValid (mFile) || !IsValid (mBuf) || !IsValid (mOs))
    {
      D_Message ("ConsoleBase: Error at initizlize sys data");
      return;
    }    
//  }
//  D_Exc ("ConsoleBase: Error at construct");  
}

ConsoleBase::~ConsoleBase ()
{
//  D_Try
//  {
    if (mOs)      delete mOs;
    if (mTempBuf) free (mTempBuf);
    if (mFile)    delete mFile;
    if (mBuf)     delete mBuf;
//  }
//  D_Exc ("ConsoleBase: Error at destruct");
}

//////////////////////implementation of class GraphConsole//////////////////////

void    GraphConsole::width   (int w)
{
  mConBase.width (w);
}

void    GraphConsole::height  (int h)
{
  mConBase.height (h);
}

GraphConsole::GraphConsole  (Pool* _Pool,ConsoleBase& _base)
             : MemObject (_Pool),
               mConBase  (_base)
{   
  mConBase.bind (this);
}

GraphConsole::~GraphConsole ()
{   
  if (mConBase.grcon() == this)
    mConBase.unbind (this);
}
