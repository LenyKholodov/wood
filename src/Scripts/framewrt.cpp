#include <pch.h>
#include <scripts\framewrt.h>
#include <binfile.h>

int FrameWriter::tagiobuf::overflow (int d)
{
  if (!buf)
    return EOF;

  switch (d)
  {
    case '\n':
      newLine = true;
      break;
    default:
      if (newLine && d != '\n' && d != '\r')          
      {  
        int count = stream.mLevel*stream.mSpaceCount;

        while (count)
        {
          int cnt = end - pos < count ? end - pos : count;          

          for (int i=0;i<cnt;i++)
            *pos++ = ' ';
            
          flush ();  

          count  -= cnt;
        }
        
        newLine = false;    
      } 
  }

  if (pos == end)
    flush ();

  return *pos++ = d;
}

void  FrameWriter::tagiobuf::flush  ()
{
  if (IsValid (file) && buf != pos)
  {
    file->write (buf,pos-buf);
    pos = buf;
    end = buf + bufSize;
  }
}

FrameWriter::tagiobuf::tagiobuf (BinFile* f,FrameWriter& s,Pool* pool,size_t size)
            : stream  (s), 
              newLine (true), 
              file    (f) ,
              buf     (NULL), pos (NULL), end (NULL),
              bufSize (0)
{  
  if (!IsValid (file))
  {
    file = NULL;
    return;
  }

  buf = new (pool) char [bufSize=size];  
  pos = buf;
  end = pos + bufSize;
}

FrameWriter::tagiobuf::~tagiobuf ()
{
  flush ();  

  if (buf)
    delete [] buf;
}

void  FrameWriter::OpenFrame  (const char* tag)
{
  (*this)<<tag<<"\n{"<<endl;    
  mLevel++;
}

bool  FrameWriter::CloseFrame ()
{
  if (mLevel)
  {
    mLevel--;

    (*this)<<'}'<<endl;

    return true;
  }
  else return false;
}

size_t FrameWriter::write (const void* buf,size_t size)
{
  return mAssign ? mAssign->write (buf,size) : 0;
}

size_t FrameWriter::write (const char* tag,const void* buf,size_t size,long offs)
{
  if (!mAssign)
    return 0;

  if (offs != -1)
    if (offs != mAssign->seek (offs))
      return 0;
  
  offs       = mAssign->getpos ();
  size_t res = mAssign->write (buf,size);

  if (!res)
    return 0;

  (*this)<<".chunk "<<tag<<" size "<<res<<" offs "<<offs;
  (*this)<<';'<<endl;

  return res;
}

FrameWriter::FrameWriter (BinFile* stream,BinFile* assign,Pool* pool,size_t size)
                   : mFile (stream),
                     mAssign (assign),
                     mDelFile (false), mDelAssign (false),
                     mLevel (0), mSpaceCount (4),
                     mBuf (stream,*this,pool,size)
{   
  ios::init (&mBuf);  
}

static uint cLevels [8] = {
  0,
  BF_OPEN_PACK_STREAM1,
  BF_OPEN_PACK_STREAM2,  
  BF_OPEN_PACK_STREAM3,
  BF_OPEN_PACK_STREAM4,
  0,
  0,
  0  
};

FrameWriter::FrameWriter (const char* stream,const char* assign,uint compress,Pool* pool,size_t size)
                   : mFile      (F_OpenFile (stream,BF_OPEN_CR|BF_OPEN_CACHE)),
                     mAssign    (assign?F_OpenFile (assign,BF_OPEN_CR|cLevels [compress&7]):NULL),
                     mDelFile   (IsValid (mFile)), 
                     mDelAssign (IsValid (mAssign)),
                     mLevel     (0), 
                     mSpaceCount (4),
                     mBuf       (mFile,*this,pool,size)
{   
  ios::init (&mBuf);  

  if (IsValid (mAssign))
  {
    (*this)<<".assign \""<<assign<<"\"";
    
    if (compress)
      (*this)<<" compress";

    (*this)<<endline;
  }  
}

FrameWriter::~FrameWriter ()
{
  if (!mDelAssign) mAssign = NULL;
  if (!mDelFile)   mFile   = NULL;
}

EndFrameLine endline;

ostream& operator << (ostream& os,const EndFrameLine&)
{
  return os<<';'<<endl;
}