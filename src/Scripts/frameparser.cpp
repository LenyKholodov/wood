#include <pch.h>
#include <scripts\framerdr.h>
#include <strstrea.h>
#include <istream.h>
#include <binfile.h>

#include "reader.h"

/////////////////////implementation of class Reader//////////////////////////////////////////

int  FrameReader::Reader::read () 
{
  if (mPos == mEnd)
  {
    if (!LoadBuffer ())
      return EOF;
  }

  int res = *mPos++;

  if (mReadString1 || mReadString2)
  {
    switch (res)
    {
      case '\r': back ();
                 *mPos++ = empty;
                 return empty;
      case '\n': back ();
                 *mPos++ = empty;
                 mLine++;
                 break;
      case '\'': if (mReadString1) mReadString1 = false;
                 break;
      case '"':  if (mReadString2) mReadString2 = false;
                 break;
    }   

    return res;
  }  
  else
  {
    switch (res)
    {
      case '\r': back (); 
                 *mPos++ = empty;
                 return empty;
      case '\n': back ();
                 *mPos++ = empty;
                 mLine++;
                 return empty;
      case '/':
      {
        int c = read ();
        if (c == '/' || c == '*')   
        {
          ReadComments (c == '/');
          return empty;
        }  
        else                        
        {
          back ();
          return res;
        }        
      }
      case '\'':
        if (!mReadString1) mReadString1 = true;
        break;
      case '"':
        if (!mReadString2) mReadString2 = true;
        break;
    }

    return res;
  }
}

void        FrameReader::Reader::back ()
{
  if (mPos > mBuf) 
    mPos--; 
  //???
}

size_t      FrameReader::Reader::LoadBuffer ()
{
  size_t size  = mFile->size () - mFile->getpos ();
  size_t rsize = 0;  

  if (size < mBufSize)    
  {
    if (size)  mFile->read (mPos=mBuf,rsize=size);
    else       
    {
      mPos = mEnd = NULL;
      return 0;
    }  
  }   
  else  
    mFile->read (mPos=mBuf,rsize=mBufSize);

  mEnd = mPos + rsize;

  return rsize;
}

void        FrameReader::Reader::OpenTag   (state_t& state)
{
//  if (mCurLevel != -1)
//  {
//    for (int c = read ();c != EOF && c != open_tag && c != close_tag;c=read ());
//
//    if (c != open_tag) error ("'{' not found"); //указать строку
//    else               mLevel++;
//  }

  state.level    = mCurLevel;
  state.openTag  = mOpenTag;
  state.closeTag = mCloseTag;
  mOpenTag       = false;
  mCloseTag      = false;
  mCurLevel      = mLevel;
}

void        FrameReader::Reader::CloseTag  (const state_t& state)
{
  if (!mCloseTag)
  {  
    if (!mOpenTag) TraceTag ();
    else 
    {
      int c = 0;
      do
      {     
        switch (c=read ())
        {
          case open_tag:  mLevel++; break;
          case close_tag: mLevel--; break;
        }            
      } while (mCurLevel != mLevel && c != EOF);
    }
  }

  mCurLevel = state.level;
  mOpenTag  = state.openTag;
  mCloseTag = state.closeTag;
}

char*   FrameReader::Reader::next  (ulong& crc)
{
  crc = 0xFFFFFFFF;

  if (!mIOBuffer.reset ())
  {
    mOpenTag = true;
    return NULL;
  }  

  char*  word = mTag;

  if (mCurLevel > mLevel)
    return NULL;

  bool   loop = true;
  int    c    = 0;

  while (loop)
  {
    c = read ();

    switch (c)
    {
      case '_': 
      case '.':
      case EOF:       loop = false;
                      break;
      case end_line:   
      case empty:     break;
      case open_tag:  mLevel++;
                      mOpenTag = true;
                      break;
      case close_tag: if (mLevel-- < 0) 
                      {
                        error ("'{' not found (error at '}'");
                        mLevel = 0;
                      }

                      if (mLevel == mCurLevel)
                      {
                        mCloseTag = true;
                        return NULL;
                      }  
                      break;
      default:        if (!isalnum (c)) error ("Syntax error: '%c'",(char)c);
                      else
                      {
                        loop = false;
                        break;
                      }
    }
  }

  if (c == EOF)
    return NULL; //конец блочного тэга

    //Чтение имени тэга

  for (;isalnum (c) || c == '_' || c == '.';c=read ())
  {
//    dout<<"!"<<(char)c<<endl;
    *word++ = c;
    crc     = CRC32 (c,crc);
  }  
  back ();

  *word = 0;
  
  return mTag;
}

void   FrameReader::Reader::TraceTag   ()
{
   //Находим первое открытие тэга, либо закрытие строчного тэга

  int  c    = 0;
  bool flag = true;

  while (flag)
  {
    switch (c=read ())
    {
      case close_tag: error ("'{' not found (error at '}')");
                      break;
      case EOF:
      case open_tag:  
      case end_line:  flag = false;
                      break;
    }  
  }

   //Тэг может быть как блочным так и в одну строку

  if (c != open_tag)
    return;

    //Найден блочный тэг (пропуск с вложением)

  int count = 1;

  while (count)
  {    
    switch (c=read ())
    {
      case open_tag:  count++;     break;
      case close_tag: count--;     break;
      case EOF:       count = 0;   break; //exit from loop
    }  
  }

  if (count)
    error ("Close tag not found (error at EOF)");
}

void FrameReader::Reader::ReadComments (bool lineComment)
{
  if (lineComment)
  {
    for (int line=mLine,c=read ();c != EOF && line == mLine;c=read ());
  }  
  else
  {
    int  cmnt   = 0;
    int  c      = 0;

    while (cmnt != 2)
    {
      switch (c=read ())
      {
        case '*':  cmnt++;
                   break;
        case '/':  if (cmnt)  cmnt++; 
                   else       cmnt = 0;
                   break;
        case EOF:  error ("*/ not found (error at EOF)"); 
                   cmnt = 2;
                   break;
        default:   cmnt = 0;
      }
    }
  }    
}

istream&    FrameReader::Reader::stream  (bool noEndLine)
{
  mStream.clear (ios::goodbit);
  return mStream;
}

void FrameReader::Reader::error  (const char* error,...)
{
  Str256   string;
  va_list  list;
  va_start (list,error);

  vsprintf (string,error,list);

  mErr<<"Line "<<mLine<<": "<<string<<endl;

  va_end (list);
}

size_t FrameReader::Reader::read  (void* buf,size_t size,long offs)
{
  if (!mAssign)
    return 0;

  if (offs != -1)
    if (offs!=mAssign->seek (offs))
      return 0;

  return mAssign->read (buf,size);
}

void   FrameReader::Reader::ChangeAssignFile (const char* fname,bool compress)
{
  BinFile* file = NULL;
  Str128   name;

  if (mAssignDir [0]) name = mAssignDir+'\\'+fname;
  else                name = fname;

  if (compress)
    file = F_OpenFile (name,BF_OPEN_RO|BF_OPEN_PACK_STREAM);
  else
    file = F_OpenFile (name,BF_OPEN_RO);

  ChangeAssignFile (file,true);
}

void  FrameReader::Reader::ChangeAssignFile (BinFile* file,bool autoDel)
{
  if (mAssignDel && mAssign)
    delete mAssign;

  if (IsValid (file))
  {
    mAssign    = file;
    mAssignDel = autoDel; 
  }
  else
  {
    if (autoDel)
      delete file;

    mAssign    = NULL;
    mAssignDel = false;
  }
}

void  FrameReader::Reader::ChangeAssignDir  (const char* dir)
{
  mAssignDir = dir;
}

FrameReader::Reader::Reader (BinFile* file,BinFile* assign,ostream& err,Pool* pool,size_t size)
                 : MemObject (pool),
                   mErr      (err),
                   mIOBuffer (*this),
                   mStream   (&mIOBuffer),
                   mLevel    (0), mCurLevel (-1),
                   mLine     (1),
                   mFile     (file), mAssign (assign),
                   mAssignDel (false),
                   mReadString1 (false), mReadString2 (false),
                   mOpenTag (false), mCloseTag (false)
{
  mBuf = ext_new char [mBufSize = size];
  mEnd = mPos = NULL;
}

FrameReader::Reader::~Reader ()
{
  if (mAssign && mAssignDel) delete mAssign;
  if (mBuf)    delete [] mBuf;
}

//////////////////implementation of class iobuffer///////////////////////////////////////////

iobuffer::iobuffer (FrameReader::Reader& rdr)
         : streambuf (NULL,0),
           mReader  (rdr), 
           mNeedClose (false), mUse (false)
{  }

int     iobuffer::underflow ()
{
  if (!mNeedClose)
    return EOF;  

  mUse = true;

  int c = mReader.read ();

  if (!mReader.mReadString1 && !mReader.mReadString2)
  {
    switch (c)
    {
      case FrameReader::end_line:  
           mNeedClose = false;
           mReader.back ();
           break;
      case FrameReader::open_tag:  
      case FrameReader::close_tag:            
           mNeedClose = false;
           mReader.back ();
           mReader.error ("';' expected (error at '%s')",(char)c);
           break;
      default:        
           return c;
    }

    return FrameReader::empty;
  }    
  else
    return c;
}
  
bool   iobuffer::reset  ()
{
  if (mNeedClose && mUse)
  {
    bool loop = true;
    int  c    = 0;

    while (loop)
    {
      switch (c=mReader.read ())
      {
        case EOF:       
        case FrameReader::end_line: 
          loop = false;
          mReader.back ();
          break;
        case FrameReader::open_tag: 
        case FrameReader::close_tag:         
          loop = false;
          mReader.back  ();
          mReader.error ("';' expected (error at '%s')",(char)c);
          break;
      }
    } 
  }

  mNeedClose = true;
  mUse       = false;

  return true;
}
