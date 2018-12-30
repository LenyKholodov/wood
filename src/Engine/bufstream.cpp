#include <pch.h>
#include <engine\bufstream.h>
#include <debug\defs.h>

bool     BufMessageStream::_LockWrite    (msg_t id)
{
  if (mWriteAvail < sizeof (message_t))
    return false;  

//  dout<<"Lock write: "<<mWriteAvail<<endl;

  message_t* msg = NULL;

  if (mWritePos + sizeof (message_t) > mBuf + mSize)
  {
    size_t size = mSize - ulong (mWritePos - mBuf) + sizeof (message_t);

    if (mWriteAvail < size)
      return false;

    msg          = (message_t*)mBuf;
    mWriteAvail -= size;
  }
  else
  {
    msg          = (message_t*)mWritePos;
    mWriteAvail -= sizeof (message_t);
  }

  msg->size      = sizeof (message_t);
  msg->msg       = id;

  mWritePos      = (uchar*)msg + sizeof (message_t);
  mWriteCur      = msg;

  return true;  
}

bool     BufMessageStream::_UnlockWrite  ()
{ 
  if (check (ST_WRITE_ERROR) && check (ST_FULL_WRITE))
  {
    mWriteAvail += mWriteCur->size;
    mWritePos    = (uchar*)mWriteCur; 
    mWriteCur    = NULL;

    return false;
  }  

  return true;
}

size_t   BufMessageStream::_write (const void* buf,size_t size)
{
  uchar*  pos = mWritePos;

  D_Try
  {
    if (size > mWriteAvail)
      size = mWriteAvail;        

//    dout<<"Write: "<<size<<endl;  

    if (mWritePos + size < mBuf + mSize)      
    {
      memcpy (mWritePos,buf,size);
      mWritePos += size;
    }  
    else
    {
      long  s = mSize - long (mWritePos - mBuf);

      memcpy (mWritePos,buf,s);
      memcpy (mBuf,(char*)buf + s,size - s);

      mWritePos = mBuf + size - s;
    }   
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("BufMessageStream: Error at _write");
    set       (ST_WRITE_ERROR);

    mWritePos = pos;    

    return 0;
  }

  mWriteAvail     -= size;
  mWriteCur->size += size;  

  return size;
}

bool     BufMessageStream::_LockRead (msg_t& id)
{
//  dout<<"Read msg count: "<<count ()<<endl;

  if (mReadPos + sizeof (message_t) > mBuf + mSize)
  {
    mWriteAvail += mSize - ulong (mReadPos - mBuf);
    mReadPos     = mBuf;
  }

  message_t* msg = (message_t*)mReadPos;

  mReadAvail     = msg->size  - sizeof (message_t); 
  mReadPos      += sizeof (message_t);
  mWriteAvail   += sizeof (message_t);

//  dout<<"Lock read: "<<mReadAvail<<endl;

  id             = msg->msg;

  return true;
}

bool     BufMessageStream::_UnlockRead ()
{
  mReadPos     += mReadAvail;
  mWriteAvail  += mReadAvail;
  mReadAvail    = 0;

  return true;
}

size_t   BufMessageStream::_read (void* buf,size_t size)
{
  uchar* pos = mReadPos;

  D_Try
  {  
    if (size > mReadAvail)
      size = mReadAvail;

//    dout<<"read "<<size<<endl;

    if (mReadPos + size < mBuf + mSize)
    {
      memcpy (buf,mReadPos,size);
      mReadPos += size;
    }  
    else
    {
      long  s = mSize - long (mReadPos - mBuf);

      memcpy (buf,mReadPos,s);
      memcpy ((char*)buf + s,mBuf,size - s);
      mReadPos = mBuf + size - s;
    }     
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("BufMessageStream: Error at _read");
    set       (ST_READ_ERROR);

    mReadPos = pos;

    return 0;
  }

//  dout<<"Read: "<<size<<endl;

  mReadAvail  -= size;
  mWriteAvail += size;

  return size;
}

size_t   BufMessageStream::_gets (char* s,bool& end)
{
  size_t size = 0;
  uchar* pos  = mReadPos;

  D_Try
  {  
    if (mReadAvail)
    {
      do
      {
        if (mReadPos < mBuf + mSize)  *s = *mReadPos++; 
        else
        {
          *s = *mBuf;
          mReadPos = mBuf + 1;
        }

        size++;
        s++;
      } while (s [-1] && size-1 < mReadAvail);

      if (!*s)
        end = true;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    D_Message ("BufMessageStream: Error at _gets");
    set       (ST_READ_ERROR);

    mReadPos = pos;

    return 0;
  }

  mReadAvail  -= size;
  mWriteAvail += size;

  return size;
}

BufMessageStream::BufMessageStream   (Pool* pool,size_t size) throw (ExcNoMemory&)
{
  mBuf = new (pool) uchar [size];

  if (!mBuf)
  {
    mWritePos  = mReadPos = NULL;
    mSize      = 0;
    mBufDel    = false;
    mWriteCur  = NULL;
    mReadAvail = mWriteAvail = 0;

    throw ExcNoMemory ("BufMessageStream",size,this);
  }   

  mSize       = size;
  mWriteAvail = size;
  mReadAvail  = 0;
  mReadPos    = mBuf;
  mWritePos   = mBuf;
  mBufDel     = true;
  mWriteCur   = NULL;    

  set (ST_READ|ST_WRITE);   
}

BufMessageStream::BufMessageStream   (void* buf,size_t size)
{
  if (!mBuf)
  {
    mWritePos  = mReadPos = NULL;
    mSize      = 0;
    mBufDel    = false;
    mWriteCur  = NULL;
    mReadAvail = mWriteAvail = 0;

    return;
  }   

  mSize       = size;
  mWriteAvail = size;
  mReadAvail  = 0;
  mReadPos    = mBuf;
  mWritePos   = mBuf;
  mBufDel     = false;
  mWriteCur   = NULL;

  set (ST_READ|ST_WRITE);
}

BufMessageStream::~BufMessageStream  ()
{
  if (mBuf && mBufDel) delete [] mBuf;
}
