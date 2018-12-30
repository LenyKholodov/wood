#include <pch.h>
#include <binfile\CacheFile.h>
#include <string.h>
#include <debug.h>

struct  CacheBinFile::buffer_t
{
  size_t             size, write_size;
  bool               dirty;
  BinFile::filepos_t pos;
  char               buf [];

  static buffer_t* create (size_t size,Pool* pool)
  {
    return (buffer_t*)pool->alloc (sizeof (buffer_t) + size);
  }                   
};

//////////////////////////implementation of class cachBinFile///////////////////

size_t  CacheBinFile::_resize (size_t size)
{
  if (size <= mFile->size ()) return size;
  else                        return mFile->resize (size);
}

size_t  CacheBinFile::_read          (void* ptr,size_t size,filepos_t from)
{
  uchar*  buf = (uchar*)ptr;
  size_t  res = 0;

  for (filepos_t pos=0;pos<size;)
  {
    buffer_t*  buffer = Find (from+pos);

    if (!buffer)  //—¨â âì ¢ ®¡å®¤ ªíè 
    {
      D_Message ("CacheBinFile: _read buffer not found");
      break;
    }  

    size_t max_size  = buffer->size - (from+pos-buffer->pos);
    size_t read_size = size-pos < max_size ? size-pos : max_size;

    memcpy (buf,buffer->buf+from+pos-buffer->pos,read_size);

    pos += read_size;
    buf += read_size;
    res += read_size;
  }

  return res; 
}

size_t  CacheBinFile::_write    (const void* ptr,size_t size,filepos_t from)
{
  uchar*  buf = (uchar*)ptr;
  size_t  res = 0;

  for (filepos_t pos=0;pos<size;)
  {
    buffer_t*  buffer = Find (from+pos);

    if (!buffer)  //¨á âì ¢ ®¡å®¤ ªíè 
    {
      D_Message ("CacheBinFile: _write buffer not found");
      break;
    }  

    size_t max_size   = buffer->size - (from+pos-buffer->pos);
    size_t write_size = size-pos < max_size ? size-pos : max_size;

    memcpy (buffer->buf+from+pos-buffer->pos,buf,write_size);

    pos += write_size;
    buf += write_size;
    res += write_size;

    if (from + pos - buffer->pos > buffer->write_size)
      buffer->write_size = from + pos - buffer->pos;

    buffer->dirty = true;  
  }

  return res;
}

size_t    CacheBinFile::WriteToDisk    (CacheBinFile::buffer_t& buffer)
{
  if (buffer.dirty)
  {
    mFile->seek  (buffer.pos);
    return buffer.size = mFile->write (buffer.buf,buffer.write_size);
  }  

  return 0;
}

size_t  CacheBinFile::ReadFromDisk   (CacheBinFile::buffer_t& buffer)
{
  mFile->seek (buffer.pos);
  return buffer.size = mFile->read (buffer.buf,buffer.size); 
}

CacheBinFile::buffer_t* CacheBinFile::Find (filepos_t fpos)
{
  if (fpos >= mBuf->pos && fpos < filepos_t (mBuf->pos + mBuf->size)) 
    return mBuf;

  if (canwrite ()) WriteToDisk (*mBuf);      

  mBuf->pos        = fpos;
  mBuf->dirty      = false;
  mBuf->write_size = 0;
  mBuf->size       = mClasterSize;
  
  if (canread ())
    mBuf->size = ReadFromDisk (*mBuf);

  if (canresize ()) mBuf->size = mClasterSize;

  return mBuf;
}

CacheBinFile::CacheBinFile   (BinFile* file,Pool* pool,size_t size,bool del)
               : MemObject    (pool),        
                 mFile        (file),
                 mClasterSize (size),
                 mBuf         (buffer_t::create (size,GetPool ())),
                 mAutoDel     (del)
{
  mFilePos = 0;
  mFileLen = file->size ();
  mMode    = file->getmode () | can_resize; 

  if (!mBuf)
    return;    

  mBuf->size        = 0;
  mBuf->write_size  = 0;
  mBuf->pos         = -1;
  mBuf->dirty       = false;
}

CacheBinFile::~CacheBinFile   ()
{
  if (mBuf) 
  {
    WriteToDisk (*mBuf);
    free (mBuf);
  }  

  if (mAutoDel)
    delete mFile;
}

//CacheBinFile::buffer_t*  CacheBinFile::Find  (filepos_t _FilePos)
//{
//  if (mLast->pos != -1 && _FilePos >= mLast->pos && _FilePos - mLast->pos < mLast->size)  
//    return mLast;
//
//  for (int i=0;i<mNum;i++)
//    if (mBuffer [i].pos != -1 && _FilePos >= mBuffer [i].pos && _FilePos - mBuffer [i].pos < mBuffer [i].size)
//      return mLast = mBuffer + i;

//  return NULL;
//}

//bool CacheBinFile::valid       () const
//{
//  if (mFile && mBuffer)
//  {
//    for (int i=0;i<mNum;i++)
//      if (!mBuffer [i].buf)
//        return false;  
//    return true;    
//  }
  //else return false;
//}