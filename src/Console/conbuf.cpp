#include <pch.h>
#include <console\conbuf.h>

////////////////////implementation of class ConsoleBuffer///////////////////////

BOOL            ConsoleBuffer::message        (const char* _msg)
{
  D_Try
  {
    if (mPos == mCount)
    {
      D_Message ("ConsoleBuffer: Console is full");
      return FALSE;
    }

    char* msg = (char*)_msg;
    int   len = strlen (msg);

    if (len < mLineLen)
    {
      mOffs [mPos++] = mFile->getpos ();

      mFile->write (msg,len);
      mFile->write ("\n",1);

      return TRUE;
    }  
    else
    {    
      char* str   = msg;
      int   pos   = 0,
            first = 0;

      for (int i=0;i<len;pos++)
      { 
        char* lineStr = str;
        first         = mLineLen;

        for (int j=0;j<mLineLen;str++,i++,j++)
        {
          if (*str == ' ')   first = j;
          if (!*str) 
          {
            mOffs   [mPos++]   = mFile->getpos ();
            
            mFile->write (lineStr,j-1);
            mFile->write ("\n",1);
            return TRUE;
          }
        }    
      
        if (mLineLen - first > (mLineLen>>2))
        {
          mOffs   [mPos++]   = mFile->getpos ();
          
          mFile->write (lineStr,mLineLen);
          mFile->write ("\n",1);
        }     
        else 
        {
          mOffs   [mPos++]   = mFile->getpos ();
          
          mFile->write (lineStr,first-1);
          mFile->write ("\n",1);
        
          str = lineStr + first;
        }        

        for (;*str == ' ';str++,i++);
      }
    }

    return TRUE;
  }
  D_ExcRet ("ConsoleBuffer: Error at message",FALSE);
}

const char*     ConsoleBuffer::string (int line,char* buf)
{
  D_Try
  {
        //Находить разность между соседними строками

    if (line < 0 || line >= mPos)
      return NULL;    

    ulong oldPos = mFile->getpos ();

    mFile->seek (mOffs [line]);
    readln      (buf);
    mFile->seek (oldPos);

    return buf;
  }
  D_ExcRet ("ConsoleBuffer: Error at string",NULL);
}

void            ConsoleBuffer::reset          ()
{
  D_Try
  {
    mPos = 0;
    mFile->reset ();
  }
  D_Exc ("ConsoleBuffer: Error at reset");
}

void            ConsoleBuffer::readln  (char* buf)
{
  char temp [128] = {0};

  while (1)
  {
    size_t size = mFile->read (temp,128);
    char*  str  = temp;

    if (!size) 
      break;

    for (int i=0;i<size;i++,str++)
      if (*str != '\n') *buf++ = *str;
      else              
      {
        *buf++ = 0;
        return;
      }  
  }

  *buf = 0;
}

ConsoleBuffer::ConsoleBuffer  (Pool* _Pool,BinFile* _File,int _LineNum,int _LineLen)
              : MemObject (_Pool),
                mFile     (_File),
                mOffs     (NULL),
                mCount    (_LineNum), mPos (0), mLineLen (_LineLen)
{
//  D_Try
//  {
    mOffs = (ulong*)alloc (sizeof (ulong) * _LineNum);

    if (!mOffs)
    {
      D_Message ("ConsoleBuffer: No enough memory for sys data");
      return;
    }

    mFile->resize (0);
//  }
//  D_Exc ("ConsoleBuffer: Error at construct");
}

ConsoleBuffer::~ConsoleBuffer ()
{
//  D_Try
//  {
    if (mOffs) free (mOffs);  
//  }
//  D_Exc ("ConsoleBuffer: Error at destruct");
}
