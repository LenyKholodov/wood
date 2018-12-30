#include <pch.h>
#include <console\conbase.h>
#include <console\command.h>

void     CommandLine::prev         ()
{
  if (mCursorPos) mCursorPos--;
}

void     CommandLine::next         ()
{
  if (mCursorPos != mPos) mCursorPos++;
}

void     CommandLine::home         ()
{
  mCursorPos = 0;
}

void     CommandLine::end          ()
{
  mCursorPos = mPos;
}

void     CommandLine::del          ()
{
  if (mCursorPos != mPos)        
  {
    memmove (mBuf + mCursorPos,mBuf + mCursorPos + 1,strlen (mBuf+mCursorPos+1)+1);
    mPos--;
  }  
}

BOOL     CommandLine::cursor       ()
{
  if (clock () - mLast > mBlink)
  {
    mCursor = !mCursor;
    mLast   = clock ();
  }

  return mCursor;
}

void     CommandLine::prevcmd      ()
{
  D_Try
  {
    const char* cmd = mHistory.up ();
    if (cmd) 
    {
      strcpy (mBuf,cmd);
      mPos = mCursorPos = strlen (mBuf);
    }  
    else
      mPos = mCursorPos = 0;
  }
  D_Exc ("CommandLine: Error at prevcmd");
}

void     CommandLine::nextcmd      ()
{
  D_Try
  {
    const char* cmd = mHistory.down ();
    if (cmd) 
    {
      strcpy (mBuf,cmd);
      mPos = mCursorPos = strlen (mBuf);
    }  
    else
      mPos = mCursorPos = 0;
  }
  D_Exc ("CommandLine: Error at nextcmd");
}

char*    CommandLine::gets () {  return mBuf; }

void     CommandLine::put (char c)
{
  switch (c)
  {
    case '\n':
      if (mPos == 0)            
      {
        mBuf [mPos++] = '\\';
        mConBase.os()<<mBuf<<endl;
        mHistory.remember (mBuf);
      }
      else
      {
        mConBase.os()<<mBuf<<endl;
        mHistory.remember (mBuf);
        mCommand.command (mBuf);      
      }

      mBuf [0]    = 0;
      mPos        = 0;
      mCursorPos  = 0;

      break;
    case 8:
      if (mPos && mCursorPos)
      {
        memmove (mBuf + mCursorPos - 1,mBuf + mCursorPos,strlen (mBuf+mCursorPos)+1);
        mCursorPos--;
        mPos--;
      }      
  
      break;
    case '\t':
    {
      char* str = mBuf;

      if (str [0] == '\\')
        str++;

      if (mCommand.vpool.helpfind (str,mConBase.os()))
      {
        mPos = mCursorPos = strlen (str) + 1;
        memmove (mBuf+1,str,mPos);
        mBuf [0] = '\\';
      }  
      break;
    }  
    default:
      if (mPos != mLen && mCursorPos == mPos)
      {
        mBuf [mPos++] = c;
        mBuf [mPos]   = 0;
        mCursorPos++;
        break;
      }
      else if (mPos < mLen-1)
      {
        memmove (mBuf + mCursorPos + 1,mBuf + mCursorPos,strlen (mBuf + mCursorPos)+1);
        mBuf [mCursorPos] = c;
        mPos++;
        break;
      }
  }
}

CommandLine::CommandLine  (Pool* _Pool,ConsoleBase& base,Command& com,int length,int depth)
            : MemObject (_Pool),
              mHistory  (_Pool,depth),
              mConBase  (base),
              mCommand  (com),
              mLen      (length), mPos (0), mCursorPos(0),
              mBuf      (NULL),
              mCursor   (FALSE),
              mLast     (clock()),
              mBlink    (CLK_TCK >> 1)
{
//  D_Try
//  {
    mBuf = (char*)alloc (mLen+8);

    if (!mBuf)
    {
      D_Message ("CommandLine: No enough memory for sys data");
      return;
    }

    mBuf [0] = 0;

    mCommand.bind (base.os());
    mConBase.bind (*this);
//  }
  //D_Exc ("CommandLine: Error at construct");
}

CommandLine::~CommandLine ()
{
//  D_Try
//  {
    if (mBuf) free (mBuf);
    mConBase.unbind (*this);
//  }
//  D_Exc ("CommandLine: Error at destruct");  
}
