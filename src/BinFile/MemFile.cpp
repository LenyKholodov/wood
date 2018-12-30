#include <pch.h>
#include <binfile\MemFile.h>
#include <pool.h>
#include <string.h>

size_t   MemBinFile::AssignFrom  (BinFile& file)
{
  if (!IsValid (file)) return file.read (mBuffer,mFileLen);  
  else                 return 0;
}

size_t   MemBinFile::AssignTo    (BinFile& file)
{
  if (!IsValid (file))    return file.write (mBuffer,mFileLen);
  else                    return 0;
}

size_t   MemBinFile::_read   (void* buf,size_t size,filepos_t pos)
{
  return memcpy (buf,mBuffer+pos,size), size;
}

size_t   MemBinFile::_write  (const void* buf,size_t size,filepos_t pos)
{
  return memcpy (mBuffer+pos,buf,size), size;
}

size_t     MemBinFile::_resize      (size_t size)
{
  if (mFileLen >= size)   mFileLen = size;
  else                    open (size,mMode);
      
  return mFileLen;
}

bool     MemBinFile::open    (size_t size,int mode)
{
  if (!mPool)
    return false;

  char* buf  = mBuffer;
  bool  own  = mOwner;

  mBuffer    = (char*)mPool->alloc  (size);

  if (!mBuffer)
    return false;

  mOwner     = true;

  if (buf)
  {
    memcpy (mBuffer,buf,size>mFileLen?mFileLen:size);

    if (own) mPool->free (buf);

    mFileLen      = 0;
    mMode         = (States)0;
  }

  mMode    = (States)mode;
  mFileLen = size;

  return true;
}

MemBinFile::MemBinFile (Pool* pool,size_t size)
           : mBuffer (NULL),
             mOwner  (true),
             mPool   (pool)
{
  open (size,can_read|can_write|can_seek|can_resize|can_reset);
}

MemBinFile::MemBinFile (void* ptr,size_t size)
           : mBuffer   ((char*)ptr),
             mOwner    (false),
             mPool     (NULL)
{ 
  mFileLen       = size;
  mFilePos       = 0;
  mMode          = can_read|can_write|can_seek|can_reset;
}

MemBinFile::~MemBinFile()
{
  if (mOwner && mPool && mBuffer) mPool->free (mBuffer);
}
