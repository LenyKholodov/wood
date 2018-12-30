void cacheBinFile::invalidatebuf(void)
{
  if(mDirty)
  {
    if(mFileSeekPos!=mFileBufPos)
      mFile->seek(mFileBufPos);

    mBufRead     = mFile->write(mBuffer,mBufRead);
    mFileSeekPos = mFileBufPos + mBufRead;
    mDirty       = FALSE;
  }
  
  mFileBufPos   += mBufRead;
  mBufRead       = 0;
  mBufPos        =0;
}

size_t cacheBinFile::_read(void *buf,size_t len)
{
  int len0 = mBufRead - mBufPos;

  if (len<len0) len0 = len;

  len      -= len0;

  memcpy(buf,mBuffer+mBufPos,len0);

  mBufPos  += len0;
  mFilePos  = mFileBufPos + mBufPos;

  if (!len)
    return len0;

  if(len < mBufLen-mBufRead)
  {
    if (mFileSeekPos != mFileBufPos + mBufRead)
       mFile->seek(mFileBufPos+mBufRead);

    int l=mFile->read(mBuffer,mBufLen-mBufRead);

    if (l<len)  len=l;

    memcpy((char*)buf+len0,mBuffer+mBufRead,len);

    mBufPos   += len;
    mBufRead  += l;
  }
  else
  {
    invalidatebuf();

    if(len >= mBufLen)
    {
      if (mFileSeekPos != mFileBufPos)
         mFile->seek(mFileBufPos);

      len = mFile->read((char*)buf+len0,len);

      mFileBufPos+=len;
    }
    else
    {
      if (mFileSeekPos != mFileBufPos)
          mFile->seek (mFileBufPos);

      mBufRead=mFile->read(mBuffer,mBufLen);

      if(mBufRead<len)  len=mBufRead;

      memcpy((char*)buf+len0,mBuffer,len);

      mBufPos = len;
    }
  }

  mFileSeekPos = mFileBufPos + mBufRead;
//  mFilePos     = mFileBufPos + mBufPos; //???
  mFilePos     = mFileBufPos + mBufPos - (len0+len);

  return len0+len;
}

size_t cacheBinFile::_write  (const void *buf,size_t len)
{
  int len0 = mBufLen - mBufPos;

  if(len<len0)    len0=len;
  if(len0)        mDirty=TRUE;

  memcpy(mBuffer+mBufPos,buf,len0);

  len     -= len0;
  mBufPos += len0;

  if (mBufPos > mBufRead)
      mBufRead = mBufPos;

  mFilePos = mFileBufPos + mBufPos;

  if (mFilePos > mFileLen)
      mFileLen = mFilePos;

  if (!len)
      return len0;

  invalidatebuf();

  if(len >= mBufLen)
  {
    if (mFileSeekPos != mFileBufPos)
        mFile->seek (mFileBufPos);

    len = mFile->write((const char*)buf+len0,len);

    mFileBufPos  += len;
    mFileSeekPos  = mFileBufPos;
  }
  else
  {
    memcpy(mBuffer,(const char*)buf+len0,len);

    mBufRead  = len;
    mBufPos   = len;
  }

  mFilePos = mFileBufPos + mBufPos;

  if (mFilePos > mFileLen)
      mFileLen=mFilePos;

  return len0+len;
}

BinFile::filepos_t cacheBinFile::seek(filepos_t pos)
{
  if (!canseek())
      return mFilePos;

  if (pos <  0)          pos = 0;
  if (pos >  mFileLen)   pos = mFileLen;
  if (pos >= mFileBufPos && pos <= mFileBufPos + mBufRead)
      mBufPos=pos-mFileBufPos;
  else
  {
    invalidatebuf();
    mFileBufPos = pos;
   }
 
   return mFilePos = pos;
}

size_t cacheBinFile::resize (size_t pos)
{
  if (!canresize())
      return BinFile::resize(pos);

  invalidatebuf();

  mFileLen = mFile->resize(pos);
  mFilePos = mFileBufPos = mFile->getpos();

  return mFileLen;
}

cacheBinFile::cacheBinFile  (BinFile* file,Pool* pool,size_t len)
             : mPool       (pool),
               mBuffer     (NULL),
               mBufLen     (0), mBufPos     (0), mBufRead    (0),
               mFileBufPos (0), mFileSeekPos (0),
               mDirty       (false),
               mFile        (file)
{
  if (!file->getmode())
      return ;

  int fmode = mFile->getmode();

  if (fmode & can_seek==0 || fmode==can_seek)
      return;

  if(!(mBuffer = (char*)mPool->alloc(len)))
    return ;

  mFileLen    = mFile->size ();
  mBufLen     = len;
  mBufPos     = mBufRead = mFilePos = 0;
  mDirty      = FALSE;
  mFileBufPos = mFileSeekPos = mFile->getpos();
  mMode       = (States)fmode;
}

cacheBinFile::~cacheBinFile ()
{
  if(mMode)
  {
    invalidatebuf ();
    mPool->free  (mBuffer);
  }
}

/*size_t    clcacheBinFile::_write         (const void *ptr,size_t len)
{
  uchar*  buf   = (uchar*)ptr;
  size_t  rlen  = 0,
          bsize = 0;          

  for (size_t size=0;size<len;)
  {
    buffer_t*  buffer = Find (mFilePos+size);

    if (!buffer)
    {    
      buffer  = &Reset (mFilePos+size);
      rlen    = len-size<buffer->size?len-size:buffer->size;

      memcpy (buffer->buf,buf,rlen);

      size   += rlen;
      buf    += rlen;

      buffer->write = rlen;
    }
    else
    {
      long l  = mFilePos-buffer->pos+size;
      bsize   = buffer->size - l;
      rlen    = len-size<bsize?len-size:bsize;

      memcpy (buffer->buf+l,buf,rlen);

      size   += rlen;
      buf    += rlen;      

      buffer->write = l+rlen > buffer->write ? l+rlen : buffer->write;
    }        

    if (!rlen)   break;
    else         buffer->dirty = TRUE;
  }

  return size;
} */

const int SEEK_BUF_SIZE = 0x10000;

static char seekBuf [SEEK_BUF_SIZE] = {0};

BinFile::filepos_t  gzBinFile::_seek   (filepos_t pos)
{
  if (z_err == Z_ERRNO || z_err == Z_DATA_ERROR) 
      return -1;
  
  if (canwrite ()) 
  {
    pos -= stream.total_in;

    if (pos < 0) return -1L; //seek назад

    while (pos > 0)  
    {
      uInt size = SEEK_BUF_SIZE;

      if (pos < SEEK_BUF_SIZE) size = (uInt)pos;


      size = write (seekBuf,size);

      if (!size) 
        return -1;

      pos -= size;
    }

    return stream.total_in;
  }
  else 
  {
    pos += stream.total_out;

    dout<<"kkk: "<<pos<<" "<<stream.total_out<<endl;

    if (pos < 0)
      return -1;

    if ((uLong)pos >= stream.total_out)   pos -= stream.total_out;
    else if (!reset ())                       
    {
      dout<<"pop"<<endl;
      return -1;
    }  

    while (pos > 0)  
    {
      int size = SEEK_BUF_SIZE;

      if (pos < SEEK_BUF_SIZE) 
        size = (int)pos;

      dout<<"jeje "<<size<<endl;

      size = read (seekBuf,size);

      if (size <= 0) 
        return -1;

      pos -= size;
    }

    return stream.total_out;
  }        
}