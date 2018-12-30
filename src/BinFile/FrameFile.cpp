#include <pch.h>
#include <binfile\FrameFile.h>

size_t FrameBinFile::_read(void *buf,size_t len,filepos_t pos)
{       
  return mFile->read (buf,len,mOffs + pos);
}

size_t FrameBinFile::_write(const void *buf,size_t len,filepos_t pos)
{
  return mFile->write (buf,len,mOffs + pos); 
}

size_t FrameBinFile::_resize (size_t size)
{
  if (size + mOffs > mFile->size ())
      size = mFile->resize (size + mOffs) - mOffs;

  return size;
}

FrameBinFile::FrameBinFile (BinFile* file,filepos_t offs,size_t len,int fm)
         : mFile   (file),
           mOffs   (offs)
{
  if (fm < 0)
    fm = mFile->getmode ();

  int fmode = mFile->getmode();

  if (!fmode || !(fmode&can_seek))
      return ;

  long l   = mFile->size();
  mFileLen = len;
  mMode    = fmode & fm;

  if (offs + len > size ())
    resize (offs + len);

  mOffs    = (mMode&can_resize) ? offs : (offs>l) ? l : offs;
}        
